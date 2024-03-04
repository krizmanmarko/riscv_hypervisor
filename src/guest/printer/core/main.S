.section ".boot", "ax"
.globl _start

_start:
	la t1, trap
	csrw stvec, t1

	li sp, 0x80101000

	li s0, 0x10000000
	li t1, 0x41
	sb t1, 0(s0)

	call set_timer

	csrr t1, sstatus
	ori t1, t1, 2		/* SIE bit enable */
	csrw sstatus, t1
	csrr t1, sie
	ori t1, t1, 0b100000
	csrw sie, t1
	
	li t1, 0x42
	sb t1, 0(s0)
suspend:
	wfi
	j suspend

.section ".text", "ax"
set_timer:
	mv t0, ra
	csrr a0, time
	li t2, 10000000
	add a0, a0, t2
	call sbi_set_timer
	mv ra, t0
	ret

send_ipi:
	mv t0, ra
	la a0, hart_mask
	call sbi_send_ipi
	mv ra, t0
	ret

.balign 4
trap:
	li t1, 0x43
	sb t1, 0(s0)
	li t1, 10	/* ACK that we reached handler */
	sb t1, 0(s0)
	call set_timer
/*	call send_ipi */
	sret

.section ".data", "aw"
hart_mask:
	.quad 2
