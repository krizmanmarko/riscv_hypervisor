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
	li a6, 0
	li a7, 0x48534d

	li t2, 0
	li t1, DTB_NR_CPUS
1:	beq t2, t1, 2f
	addi t1, t1, -1
	beq tp, t1, 1b	# if (current) continue;
	mv a0, t1
	la a1, enter_vas
	mv a2, t0
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
# when kernel image exceeds 1GB -> rework

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
.globl relocate_stack

init_for_c:
	# init stack - last used, grows towards smaller, 16-bit aligned
	la t0, cpus
	li t1, CPU_SIZE
	addi t2, a0, 1
	mul t2, t1, t2
	add sp, t0, t2

	# finally goto C
	j main

# void relocate_stack(uint64 offset);
relocate_stack:
	# first change sp and fp
	add sp, sp, a0
	add fp, fp, a0

	# only after stack is relocated modify stored fp (on stack)
	# and since main is the first function using stack, we do not have to
	# change any other frames' stored fp
	ld a1, -16(fp)
	add a1, a1, a0
	sd a1, -16(fp)
	ret
