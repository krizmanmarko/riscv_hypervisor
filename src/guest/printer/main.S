.section ".text", "ax"
.globl _start

_start:
	li t0, 0x10000000
	li t1, 0x41
	sb t1, 0(t0)
	sb t1, 0(t0)
	sb t1, 0(t0)
	sb t1, 0(t0)
	sb t1, 0(t0)
	sb t1, 0(t0)
	sb t1, 0(t0)
	sb t1, 0(t0)
	j .
