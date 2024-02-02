.section ".boot", "ax"
.globl _start

_start:
	la t1, trap
	csrw stvec, t1

	li sp, 0x80000800

	li t0, 0x10000000
	li t1, 0x41
	sb t1, 0(t0)

	call set_timer

	csrr t1, sstatus
	ori t1, t1, 2		/* SIE bit enable */
	csrw sstatus, t1
	csrr t1, sie
	ori t1, t1, 0b100000
	csrw sie, t1
	
	li t1, 0x42
	sb t1, 0(t0)
	j .

.section ".text", "ax"
set_timer:
	csrr a0, time
	li t2, 10000000
	add a0, a0, t2
	call sbi_set_timer
	ret

.balign 4
trap:
	li t1, 0x43
	sb t1, 0(t0)
	li t1, 10	/* ACK that we reached handler */
	sb t1, 0(t0)
	call set_timer
	sret
