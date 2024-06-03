.section ".boot", "ax"
.globl _start

_start:
	la t1, trap
	csrw stvec, t1

	li sp, 0x80101000
	csrr t1, sstatus
	ori t1, t1, 2		/* SIE bit enable */
	csrw sstatus, t1
	csrr t1, sie
	ori t1, t1, 0b1000000000	/* sie.seie */
	csrw sie, t1

	call init_uart
	li a0, 0x44
	call uartputc
	li a0, 0x44
	call uartputc
	li a0, 0x44
	call uartputc

suspend:
	wfi
	j suspend

.section ".text", "ax"

.balign 4
trap:
	lb a0, 0(s0)
	call uartputc
	li a0, 10
	call uartputc
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
