#include "dtb.h"
#include "mem.h"

// Sv39 specific macros

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


.section ".boot.bsp", "aw"

.balign PAGE_SIZE
bsp_root: .zero PAGE_SIZE	# boot strap pages root


.section ".boot.text", "ax"

.globl early_mem_prot_init
early_mem_prot_init:
	mv a1, ra
	call init_bootstrap_pgtables
	call enter_vas
	mv ra, a1
	ret

init_bootstrap_pgtables:
	# TODO: gigapage round down
	MAP_GIGAPAGE bsp_root, DTB_MEMORY, DTB_MEMORY
	MAP_GIGAPAGE bsp_root, VAS_KERNEL, DTB_MEMORY
	MAP_GIGAPAGE bsp_root, VAS_RAM, DTB_MEMORY
	ret

enter_vas:
	li t1, 1
	slli t1, t1, 63
	la t2, bsp_root
	srli t2, t2, 12
	or t1, t1, t2
	csrw satp, t1
	# sfence.vma	# not necessary because TLB was not used yet
	ret
