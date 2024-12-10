.section ".boot", "ax"

# Trenuten setup ima na voljo 512KB spomina (0x000000 - 0x80000). UART se začne na naslovu 0x10700000.

.globl setup
setup:
	# install exception handler as early as possible
	la t0, early_handler
	csrw stvec, t0

	# store hartid in tp (for early boot) <- TODO: should be in CPU struct
	mv tp, a0

	call early_mem_prot_init
#	call init_for_c
#	j early_boot	# C code
#	call mem_prot_init

	j .

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
