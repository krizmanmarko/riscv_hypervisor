#include <cpu.h>

.section ".boot", "ax"

# Trenuten setup ima na voljo 512KB spomina (0x000000 - 0x80000). UART se začne na naslovu 0x10700000.

.globl setup
setup:
	# install exception handler as early as possible
	la t0, early_handler
	csrw stvec, t0

	# store hartid in sscratch (for early boot)
	csrw sscratch, a0

	call early_mem_prot_init
	call init_for_c
	j early_boot
#	call mem_prot_init

	j .

init_for_c:
	# init stack - last used, grows towards smaller, 16-byte aligned
	la sp, cpu_stacks
	li a0, CPU_STACK_SIZE
	csrr a1, sscratch
	add a1, a1, 1
	mul a0, a0, a1
	add sp, sp, a0
	ret

.balign 4
early_handler:
	j .




/*
##### EARLY BOOT PROCESS (master hart only) #####

call early_mem_prot_init
	MMU:	bootstrap pgtables + enter VAS
	MPU:	nop
j init_for_C
j main
call init_uart
call mem_prot_init
	MMU:	kmem
		vmem
		final pgtables + relocate stack
		build pgtables for other harts + wake them up
	MPU:	PMP
*/
