// TODO: use console read write instead of direct uart

// fmt: "%{format}{length}{type}"
// format: {_ - decimal, b - binary, x - hex}
// length: {l, _, h, hh}
// type: {u - unsigned, d - signed, p - pointer, %, s - string}


#include <stdarg.h>
#include "types.h"

void uartputc(char c);
void printf(char *fmt, ...);

static char digits[] = "0123456789abcdef";

static void
print_uint(uint64 num, int base)
{
	int size;
	uint64 tmp;
	char buf[64];

	// TODO print error
	if (base < 2) return;	// otherwise overflow is possible

	tmp = num;
	size = 0;
	do {
		buf[size++] = digits[tmp % base];
		tmp /= base;
	} while (tmp > 0);

	while (--size >= 0)
		uartputc(buf[size]);
}

static void
print_padded(uint64 num, int base, int bytes)
{
	int chars_for_byte;	// 0xff represents 1 byte (2 chars_for_byte)
	int bits_in_char;	// 0xf -> 4 bits_in_char
	int start, end;

	if (base != 2 && base != 16)
		return;	// TODO panic (invalid base)

	bits_in_char = 0;
	while (base > 1) {
		base /= 2;
		bits_in_char++;
	}
	chars_for_byte = 8 / bits_in_char;

	start = (sizeof(uint64) - bytes) * chars_for_byte;
	end = sizeof(uint64) * chars_for_byte;
	for (int i = 0; i < start; i++)
		num <<= bits_in_char;
	for (int i = start; i < end; i++, num <<= bits_in_char)
		uartputc(digits[num >> (sizeof(uint64) * 8 - bits_in_char)]);
}

void
printf(char *fmt, ...)
{
	char c;
	int i;
	va_list ap;
	int base, length;
	char *s;

	if (fmt == 0) return;	// TODO print error

	va_start(ap, fmt);
	c = fmt[0];
	i = 0;
	while (c != 0) {
		if (c != '%') {
			uartputc(c);
			c = fmt[++i];
			continue;
		}
		c = fmt[++i];
		base = 10;
		length = sizeof(uint32);
// decide_format:
		switch (c) {
		case 'b':
			base = 2;
			c = fmt[++i];
			goto decide_length;	// aka break
		case 'x':
			base = 16;
			c = fmt[++i];
			goto decide_length;
		default:
			goto decide_length;
		}
decide_length:
		switch (c) {
		case 'l':
			length *= 2;
			c = fmt[++i];
			goto decide_type;
		case 'h':
			length /= 2;
			c = fmt[++i];
			if (c == 'h') {
				length /= 2;
				c = fmt[++i];
			}
			goto decide_type;
		default:
			goto decide_type;
		}
decide_type:
		switch (c) {
		case '%':
			uartputc('%');
			break;
		case 'p':
			uartputc('0');
			uartputc('x');
			print_padded(va_arg(ap, uint64), 16, sizeof(void *));
			break;
		case 's':
			s = va_arg(ap, char *);
			if (s == 0)
				s = "(null)";
			while (*s != 0) {
				uartputc(*s);
				s++;
			}
			break;
		case 'd':
			switch (length) {
			case 1:
				int8 tmp8 = va_arg(ap, int);
				if (tmp8 < 0) {
					uartputc('-');
					tmp8 = -tmp8;
				}
				if (base == 2 || base == 16)
					print_padded(tmp8, base, length);
				else
					print_uint(tmp8, base);
				break;
			case 2:
				int16 tmp16 = va_arg(ap, int);
				if (tmp16 < 0) {
					uartputc('-');
					tmp16 = -tmp16;
				}
				if (base == 2 || base == 16)
					print_padded(tmp16, base, length);
				else
					print_uint(tmp16, base);
				break;
			case 4:
				int32 tmp32 = va_arg(ap, int32);
				if (tmp32 < 0) {
					uartputc('-');
					tmp32 = -tmp32;
				}
				if (base == 2 || base == 16)
					print_padded(tmp32, base, length);
				else
					print_uint(tmp32, base);
				break;
			case 8:
				int64 tmp64 = va_arg(ap, int64);
				if (tmp64 < 0) {
					uartputc('-');
					tmp64 = -tmp64;
				}
				if (base == 2 || base == 16)
					print_padded(tmp64, base, length);
				else
					print_uint(tmp64, base);
				break;
			default:
				printf("printf: invalid length\n");
				break;
			}
			break;
		case 'u':
			switch (length) {
			case 1:
				uint8 tmp8 = va_arg(ap, unsigned int);
				if (base == 2 || base == 16)
					print_padded(tmp8, base, length);
				else
					print_uint(tmp8, base);
				break;
			case 2:
				uint16 tmp16 = va_arg(ap, unsigned int);
				if (base == 2 || base == 16)
					print_padded(tmp16, base, length);
				else
					print_uint(tmp16, base);
				break;
			case 4:
				uint32 tmp32 = va_arg(ap, uint32);
				if (base == 2 || base == 16)
					print_padded(tmp32, base, length);
				else
					print_uint(tmp32, base);
				break;
			case 8:
				uint64 tmp64 = va_arg(ap, uint64);
				if (base == 2 || base == 16)
					print_padded(tmp64, base, length);
				else
					print_uint(tmp64, base);
				break;
			default:
				printf("printf: invalid length\n");
				break;
			}
			break;
		default:
			printf("printf: unknown specifier\n");
			break;
		}
		c = fmt[++i];
	}
	va_end(ap);
	return;
}

void
panic(char *s)
{
	printf("panic: %s\n", s);
	while (1);
}
