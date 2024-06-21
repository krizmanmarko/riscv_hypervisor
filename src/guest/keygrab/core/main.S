.section ".boot", "ax"
.globl _start

#define CONTEXT 1
#define DTB_SERIAL 0x10000000

_start:
	mv tp, a0
	la t1, trap
	csrw stvec, t1

	li sp, 0x80100ff8
	li t1, 1
	sll t1, t1, 1
	csrs sstatus, t1	/* sstatus.sie = 1 */
	li t1, 1
	sll t1, t1, 9
	csrs sie, t1		/* sie.seie = 1 */

	# init uart
	call init_uart
	li a0, 0x44
	call uartputc

	# init plic (hartid=0)
	li a0, 10
	li a1, 1
	call plic_set_priority
	sll a0, tp, 1
	add a0, a0, 1
	li a1, 10
	li a2, 1
	call plic_set_enabled
	sll a0, tp, 1
	add a0, a0, 1
	li a1, 0
	call plic_set_threshold

suspend:
	wfi
	j suspend

.section ".text", "ax"

.balign 4
trap:
	sll a0, tp, 1
	add a0, a0, 1
	call plic_claim
	beq zero, a0, 1f	# if (claimed == 0) return
	mv t0, a0

	li s0, DTB_SERIAL
	lb a0, 0(s0)
	call uartputc
	sll a0, tp, 1
	add a0, a0, 1
	mv a1, t0
	call plic_complete

	li a0, 10
	call uartputc
1:
	sret


/*
uart is on IRQ 10 (check out dtb)

serial@10000000 {
	interrupts = <0x0a>;
	interrupt-parent = <0x07>;
	clock-frequency = <0x384000>;
	reg = <0x00 0x10000000 0x00 0x100>;
	compatible = "ns16550a";
};

*/
