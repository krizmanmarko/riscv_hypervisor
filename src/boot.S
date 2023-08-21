.section ".text"
.globl boot

boot:
	# Previous boot stage provides:
	#	a0: hart id
	#	a1: dtb

	# set up stack pointer for C
	# last used, grows towards smaller address, 16-bit aligned
	# sp = stack0 + ((hartid + 1) * 4096)
	/*
		+-------+ <- stack0
		|	|
		|	|
		+-------+ <- sp (hart 0) = stack0 + 1 * 4096
		|	|
		|	|
		+-------+ <- sp (hart 1) = stack0 + 2 * 4096
		|	|
		|	|
		+-------+ <- sp (hart 2) = stack0 + 3 * 4096
	*/
	la sp, stack0
	li t0, 4096
	addi t1, a0, 1
	mul t0, t1, t0
	add sp, sp, t0
	call start
halt:
	j	halt
