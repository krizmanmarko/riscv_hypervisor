.section ".boot", "ax"
.globl boot

# 4096 stands for CPU_STACK_SIZE

boot:
	# Previous boot stage provides:
	#	a0: hart id
	#	a1: dtb

	# set up stack pointer for C
	# last used, grows towards smaller address, 16-bit aligned
	# sp = cpu_stacks + ((hartid + 1) * 4096)
	/*
		+-------+ <- cpu_stacks
		|	|
		|	|
		+-------+ <- sp (hart 0) = cpu_stacks + 1 * 4096
		|	|
		|	|
		+-------+ <- sp (hart 1) = cpu_stacks + 2 * 4096
		|	|
		|	|
		+-------+ <- sp (hart 2) = cpu_stacks + 3 * 4096
	*/
	mv tp, a0
	la sp, cpu_stacks
	li t0, 4096
	addi t1, a0, 1
	mul t0, t1, t0
	add sp, sp, t0
	call start
halt:
	j	halt
