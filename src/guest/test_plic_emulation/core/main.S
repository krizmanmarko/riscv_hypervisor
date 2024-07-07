.section ".boot", "ax"
.globl _start

_start:
	li x2, 0xc002000
	li x1, 0x61626364
	nop

.option push		# Push current option to the stack
.option arch, -c	# Disable compressed extension

test_loads:
	#lb x1, 8(x2)	# plic access must be 32-bit
	#lbu x1, 8(x2)	# plic access must be 32-bit
	#lh x1, 8(x2)	# plic access must be 32-bit
	#lhu x1, 8(x2)	# plic access must be 32-bit
	lw x1, 8(x2)
	lwu x1, 8(x2)
	#ld x1, 8(x2)	# plic access must be 32-bit

	#flw f1, 8(x2)
	#fld x1, 8(x2)	# plic access must be 32-bit
	#flq x1, 8(x2)	# plic access must be 32-bit
	#flh x1, 8(x2)	# plic access must be 32-bit
test_stores:
	#sb x1, 8(x2)	# plic access must be 32-bit
	#sh x1, 8(x2)	# plic access must be 32-bit
	sw x1, 8(x2)
	#sd x1, 8(x2)	# plic access must be 32-bit
	fsw f1, 8(x2)
	#fsd x1, 8(x2)	# plic access must be 32-bit
	#fsq x1, 8(x2)	# plic access must be 32-bit
	#fsh x1, 8(x2)	# plic access must be 32-bit
test_lr_sc_amo:
	lr.w x1, (x2)
	sc.w x0, x1, (x2)
	amoswap.w.aq x0, x0, (x2)
	nop
.option pop		# restore previous options

test_loads_c:
	lw x1, 8(x2)
	lwu x1, 8(x2)
	flw f0, 8(x2)
test_stores_c:
	sw x1, 8(x2)
	fsw f1, 8(x2)
	#fsq x1, 8(x2)
	#fsh x1, 8(x2)
test_lr_sc_amo_c:
	lr.w x1, (x2)
	sc.w x0, x1, (x2)
	amoswap.w.aq x0, x0, (x2)

loop:	j loop
