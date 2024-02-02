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

.macro IDX2PTR root, i
	slli \i, \i, 3
	add \i, \root, \i
.endm

.section ".boot.text.s", "ax"
.globl setup

# a0: hart id
# a1: dtb
setup:
	# in case we trigger an exception in boot code
	la t0, sboot_fail
	csrw stvec, t0

	call init_bootstrap_pages

enter_vas:
	# convert init_for_c PA to VA
	la t0, eboot
	li t2, DTB_MEMORY
	li t3, VAS_BASE
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
	jr t0		# t0=PA2VA(init_for_c)

# va -> pa
# [0x80000000, dynamic] -> [0x80000000, dynamic] (just use gigapage (easy to unmap)
# [0xffffffc000000000, pa2va(dynamic)] -> [0x80000000, dynamic]
# [pa2va(dynamic), max_va] -> [dynamic, max_ram]

# i need identity mapping to perform switching vas
# i want to map hypervisor to 0xffffffc000000000, .+KERNEL_SIZE
# i want every cpu struct to map at 0xffffffc040000000, .+sizeof(struct cpu)
# i want dynamic to be at 0xffffffe000000000
# max_va 0xfffffff000000000
# we need
#	root pt (with following entries):
#		# kernel image
#		root[256][0]
#			root[256][0][0]
#			...
#			root[256][0][i] (depending on kernel size)
#		# cpu stack
#		root[257][0]
#			root[257][0][0]
#			...
#			root[257][0][j] (depending on cpu stack size)
#
#		# dynamic (root[384])
#		root[384][0]
#			root[384][0][0]
#			...
#			root[384][0][k] (depending on leftover memory)

# WARNING!
# if kernel image ever exceeds 1GB this must be reworked

# map 2 gigapages (it just avoids dynamic memory management for now)
# 0x80000000 -> 0x80000000
# VAS_BASE   -> 0x80000000
# DTB_SERIAL -> DTB_SERIAL
init_bootstrap_pages:
	# t0 <- pte
	# t1 <- root_pt[i] address

	li t0, DTB_MEMORY
	la t2, bsp_root
	PTEGEN t0, PTE_R | PTE_W | PTE_X

	# bsp_root[2]   -> DTB_MEMORY
	li t1, 2
	IDX2PTR t2, t1
	sd t0, 0(t1)

	# bsp_root[256] -> DTB_MEMORY
	li t1, 256
	IDX2PTR t2, t1
	sd t0, 0(t1)

	# bsp_root[0] -> 0 (includes DTB_SERIAL)
	li t0, 0
	PTEGEN t0, PTE_R | PTE_W
	li t1, 0
	IDX2PTR t2, t1
	sd t0, 0(t1)

	ret

.balign 4
sboot_fail:
	j .


.section ".boot.data", "aw"
.globl bsp_root
.globl init_for_c_sym

.balign 8
init_for_c_sym: .8byte text

.section ".boot.bsp", "aw"
.balign PAGE_SIZE
# boot strap pages root
bsp_root: .zero PAGE_SIZE

.section ".text.entry", "ax"
init_for_c:
	# store hartid in tp
	mv tp, a0

	# init stack - last used, grows towards smaller, 16-bit aligned
	la t0, cpus
	li t1, CPU_SIZE
	addi t2, tp, 1
	mul t1, t1, t2
	add sp, t0, t1

	# init gp (not actually necessary, boost performance)
	.option push
	.option norelax
	la gp, __global_pointer$
	.option pop

	# finally goto C
	call main

	# should never be reached
	j .
