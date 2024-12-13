.section ".text", "ax"

# uartputc(char c)
.globl uartputc
uartputc:
	li a7, 1
	ecall 		# sbi_console_putchar(char c)
	bne a0, zero, uart_fail
	ret

uart_fail:
	j .
