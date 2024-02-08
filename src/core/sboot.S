# S-mode entry point

# goals for this section:
#	setup bootstrap page table (with identity mapping)
#	enter vas
#	setup everything necessary to enter C code (stack, gp)

#include "dtb.h"
#include "memory.h"
#include "structs_in_asm.h"

.macro PTEGEN pa, flags
	# PA2PTE(pa)
	srli \pa, \pa, 12
	slli \pa, \pa, 10
	
	or \pa, \pa, \flags | PTE_V | PTE_G | PTE_A | PTE_D

	# clear top 10 bits without tmp reg
	slli \pa, \pa, 10
	srli \pa, \pa, 10
.endm

# \i = \root + \i * 8
.macro IDX2PTR root, i
	slli \i, \i, 3
	add \i, \root, \i
.endm

# pgtable[va2idx(addr)] = pte(pa, RWX)
.macro MAP_GIGAPAGE pgtable, addr, pa
	# generate pte
	li t0, \pa
	PTEGEN t0, PTE_R | PTE_W | PTE_X
	# get index
	li t1, (\addr >> 12 >> 9 >> 9) & 0x1ff
	# pgtable[index] = pte
	la t2, \pgtable
	IDX2PTR t2, t1
	sd t0, 0(t1)
.endm

.section ".boot.data", "aw"
.globl bsp_root
.globl init_for_c_sym

.balign 8
init_for_c_sym: .8byte text
wait_for_master: .byte 1

.section ".boot.bsp", "aw"
.balign PAGE_SIZE
# boot strap pages root
bsp_root: .zero PAGE_SIZE


.section ".boot.text.s", "ax"
.globl setup

# a0: hart id
# a1: dtb
setup:
	# install exception handler early
	la t0, sboot_fail
	csrw stvec, t0

	call init_bootstrap_pages

	# awaken other harts
	mv tp, a0
	mv t0, a1
	la a1, enter_vas
	mv a2, t0
	li a6, 0
	li a7, 0x48534d

	li t2, 1
	li t1, DTB_NR_CPUS
1:	beq t2, t1, 2f
	addi t1, t1, -1
	mv a0, t1
	ecall
	j 1b
2:	mv a0, tp
	mv a1, t0

enter_vas:
	# store hartid in tp
	mv tp, a0

	# PA2KVA(init_for_c) by hand
	la t0, eboot
	li t2, DTB_MEMORY
	li t3, VAS_KERNEL
	sub t0, t0, t2
	add t0, t0, t3

	# setup satp (ATP_MODE_Sv39 | (bsp_root >> 12))
	li t1, 1
	slli t1, t1, 63
	la t2, bsp_root
	srli t2, t2, 12
	or t1, t1, t2
	csrw satp, t1
	sfence.vma
	jr t0		# t0=PA2KVA(init_for_c)

# WARNING!
# kernel image exceeds 1GB -> rework

# map gigapages (without dynamic memory management)
init_bootstrap_pages:
	MAP_GIGAPAGE bsp_root, 0, 0	# this is for DTB_SERIAL
	MAP_GIGAPAGE bsp_root, DTB_MEMORY, DTB_MEMORY
	MAP_GIGAPAGE bsp_root, VAS_KERNEL, DTB_MEMORY
	MAP_GIGAPAGE bsp_root, VAS_RAM, DTB_MEMORY
	ret

.balign 4
sboot_fail:
	j .

.section ".text.entry", "ax"
# TODO: this is still trash, where is stack going to be?
# where is synchronisation between harts
# do not store hartid in tp but rather in cpu struct
# map cpu struct that also has stack

init_for_c:
	# init stack - last used, grows towards smaller, 16-bit aligned
	la t0, cpus
	li t1, CPU_SIZE
	addi t2, a0, 1
	mul t2, t1, t2
	add sp, t0, t2

	# store hartid in cpu struct
	mul t2, t1, a0
	add t2, t0, t2
	sw a0, CPU_HARTID_OFF(t2)

	# init gp (not actually necessary, boost performance)
	.option push
	.option norelax
	la gp, __global_pointer$
	.option pop

	# finally goto C
	j main
