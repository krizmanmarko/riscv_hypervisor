.section ".boot", "ax"
.globl _start

_start:
	mv tp, a0
	la t1, trap
	csrw stvec, t1

	li sp, 0x80101000

	call print_hartid

	call set_timer

	csrr t1, sstatus
	ori t1, t1, 2		/* SIE bit enable */
	csrw sstatus, t1
	csrr t1, sie
	ori t1, t1, 0b100000
	csrw sie, t1
suspend:
	wfi
	j suspend

.section ".text", "ax"
print_hartid:
	li s0, 0x10000000
	add t1, tp, 48	# print ascii hartid
	sb t1, 0(s0)
	li t1, '\n'
	sb t1, 0(s0)
	ret

set_timer:
	mv t0, ra
	csrr a0, time
	li t2, 10000000
	add a0, a0, t2
	csrw stimecmp, a0
	mv ra, t0
	ret

.balign 4
trap:
	call print_hartid
	call set_timer
	sret
