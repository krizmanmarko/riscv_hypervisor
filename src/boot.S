# M-mode entry point

#include "dtb.h"
#include "memory.h"
#include "riscv_hypervisor.h"

# reg: store value here
# i: index of pmpcfg entry (pmp_i_cfg)
# flags: pmpcfg flags
.macro PMPCFG_INIT reg, i, flags
	or t1, zero, \flags
	slli t1, t1, \i * 8
	or \reg, \reg, t1
.endm

# reg: which pmpaddr_i to store to
# end: TOR end address
.macro PMPADDR_INIT reg, end
	ld t0, \end
	srli t0, t0, 2
	csrw \reg, t0
.endm

# reg: store value to this register
# val: value to or with
.macro ORI64 reg, val
	li t1, \val
	or \reg, \reg, t1
.endm

.section ".boot.data", "aw"
.balign 8
# TODO: setup pmp correctly, dynamic must probably be rwx if vms can use it
text_sym: .8byte VA2PA(text)
rodata_sym: .8byte VA2PA(rodata)
data_sym: .8byte VA2PA(data)
memory_sym: .8byte DTB_MEMORY
memory_end_sym: .8byte DTB_MEMORY_END
max_address_sym: .8byte 0xffffffffffffff	# 56-bits

.section ".boot.text.m", "ax"
.globl boot

boot:
	la t0, text_sym
	# first instruction is here
	# Previous boot stage provides:
	#	a0: hart id
	#	a1: dtb

	# setup trap vector early
	la t0, mtrapvec		# implicit direct mode (alignment)
	csrw mtvec, t0

	call setup_pmp
	call delegate_exceptions
	call delegate_interrupts
	call initialize_supervisor
	call prepare_for_mret

	mret


# Physical memory protection
#
# mmio			[0x00000000, text)		rw-
# .boot, .text		[text, rodata)			r-x
# .rodata		[rodata, data)			r--
# .data, dynamic	[data, DTB_MEMORY_END)		rw-
# leftover		[DTB_MEMORY_END, max address)	---

setup_pmp:
	# TODO: set this up when you are done changing memory layout...

	# full permissions for now
#	PMPADDR_INIT pmpaddr0, max_address_sym
#	PMPCFG_INIT t0, 0, PMPCFG_A_TOR | PMPCFG_R | PMPCFG_W | PMPCFG_X
#	csrw pmpcfg0, t0

	PMPADDR_INIT pmpaddr0, memory_sym		# end of mmio
	PMPADDR_INIT pmpaddr1, memory_end_sym		# end of memory

	mv t0, zero
	# mmio
	PMPCFG_INIT t0, 0, PMPCFG_A_TOR | PMPCFG_R | PMPCFG_W
	# ram
	PMPCFG_INIT t0, 1, PMPCFG_A_TOR | PMPCFG_R | PMPCFG_W | PMPCFG_X
	csrw pmpcfg0, t0

	ret

delegate_exceptions:
	mv t0, zero
	# delegate everything but M-mode exceptions (ECALL_FROM_M)
	ORI64 t0, MEDELEG_INSTRUCTION_ADDR_MISALIGNED
	ORI64 t0, MEDELEG_INSTRUCTION_ACCESS_FAULT
	ORI64 t0, MEDELEG_ILLEGAL_INSTRUCTION
	ORI64 t0, MEDELEG_BREAKPOINT
	ORI64 t0, MEDELEG_LOAD_ADDR_MISALIGNED
	ORI64 t0, MEDELEG_LOAD_ACCESS_FAULT
	ORI64 t0, MEDELEG_STORE_OR_AMO_ADDRESS_MISALIGNED
	ORI64 t0, MEDELEG_STORE_OR_AMO_ACCESS_FAULT
	ORI64 t0, MEDELEG_ECALL_FROM_U
	ORI64 t0, MEDELEG_ECALL_FROM_HS
	ORI64 t0, MEDELEG_ECALL_FROM_VS
	ORI64 t0, MEDELEG_INSTRUCTION_PAGE_FAULT
	ORI64 t0, MEDELEG_LOAD_PAGE_FAULT
	ORI64 t0, MEDELEG_STORE_OR_AMO_PAGE_FAULT
	ORI64 t0, MEDELEG_INSTRUCTION_GUEST_PAGE_FAULT
	ORI64 t0, MEDELEG_LOAD_GUEST_PAGE_FAULT
	ORI64 t0, MEDELEG_VIRTUAL_INSTRUCTION
	ORI64 t0, MEDELEG_STORE_OR_AMO_GUEST_PAGE_FAULT;
	csrw medeleg, t0
	# 0xf0b7ff
	ret

delegate_interrupts:
	mv t0, zero
	# delegate everything but M-mode interrupts
	ORI64 t0, MIDELEG_SSI
	ORI64 t0, MIDELEG_VSSI
	ORI64 t0, MIDELEG_STI
	ORI64 t0, MIDELEG_VSTI
	ORI64 t0, MIDELEG_SEI
	ORI64 t0, MIDELEG_VSEI
	ORI64 t0, MIDELEG_SGEI
	csrw mideleg, t0
	# 0x1666
	ret

initialize_supervisor:
	# disable interrupts and address translation
	csrw sstatus, zero
	csrw sie, zero
	csrw sip, zero
	csrw satp, zero
	ret


prepare_for_mret:
	csrr t0, mstatus
	# Previous privilege mode = S
	li t1, ~MSTATUS_MPP
	and t0, t0, t1
	li t1, STATUS_MPP_S
	or t0, t0, t1
	# Previous virtualization = 0
	li t1, ~MSTATUS_MPV
	and t0, t0, t1
	csrw mstatus, t0

	# mepc = setup
	la t0, setup
	csrw mepc, t0

	ret
