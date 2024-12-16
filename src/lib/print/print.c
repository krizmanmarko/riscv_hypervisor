
// %%: %
// %p: xlen_t pointer
// %s: string

#include <stdarg.h>
#include "arch/types.h"
#include "print.h"
#include "uart.h"

static const char digits[] = "0123456789abcdef";

static void
putptr(uintptr_t p)
{
	uintptr_t nibble;

	for (int i = sizeof(uintptr_t) * 2 - 1; i >= 0; i--) {
		nibble = (p >> i * 4) & 0x0f;
		uartputc(digits[nibble]);
	}
}

static void
puts(const char *s)
{
	int len;

	if (s == 0)
		s = "(null)";

	len = 0;
	while (s[len])
		uartputc(s[len++]);
}

static void
printf(const char *fmt, va_list ap)
{
	char c;
	int len;
	int unknown_specifier;

	if (fmt == 0)
		oops("fmt cannot be null");

	c = fmt[0];
	len = 0;
	unknown_specifier = 0;

	while (fmt[len]) {
		if (fmt[len] != '%') {
			uartputc(fmt[len++]);
			continue;
		}
		len++;	// this char is %, next is specifier
		switch (fmt[len]) {
		case '%':
			uartputc(fmt[len]);
			break;
		case 'p':
			uartputc('0');
			uartputc('x');
			putptr(va_arg(ap, uintptr_t));
			break;
		case 's':
			puts(va_arg(ap, char *));
			break;
		default:
			unknown_specifier = 1;
			puts("invalid specifier");
			break;
		}
		len++;	// this is specifier, next is unknown
	}
}

void
log(const char *fmt, ...)
{
	va_list pa;
	puts("[+] ");
	va_start(pa, fmt);
	printf(fmt, pa);
	va_end(pa);
	uartputc('\n');
}

void
oops(const char *fmt, ...)
{
	va_list pa;
	puts("\033[38;2;200;200;0m[i] oops: ");
	va_start(pa, fmt);
	printf(fmt, pa);
	va_end(pa);
	uartputc('\n');
}

void
panic(const char *fmt, ...)
{
	va_list pa;
	puts("\033[38;2;255;50;50m[!] panic: ");
	va_start(pa, fmt);
	printf(fmt, pa);
	va_end(pa);
	uartputc('\n');

	while (1);
}
