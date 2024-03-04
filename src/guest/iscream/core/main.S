.section ".boot", "ax"
.globl _start

_start:
	la t1, trap
	csrw stvec, t1

1:	wfi	# waiting for IPI
	j 1b

.section ".text", "ax"

.balign 4
trap:
	li s0, 0x10000000
	li t1, 0x41
	sb t1, 0(s0)
	sret
