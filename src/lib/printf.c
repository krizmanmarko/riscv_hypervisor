// TODO: use console read write instead of direct uart

// printf(char *, ...) -> variadic function
// fmt: "%{format}{length}{type}"
// format: {_ - decimal, b - binary, x - hex}
// length: {l, _, h, hh}
// type: {u - unsigned, d - signed, p - pointer, %, s - string}

#include <stdarg.h>
#include "lock.h"
#include "stdio.h"
#include "defs.h"
#include "types.h"

#define PRINT_UINT(uvar, utype) \
	do { \
		uvar = va_arg(ap, utype); \
		if (base == 2 || base == 16) \
			print_padded(uvar, base, length); \
		else \
			print_uint(uvar, base); \
	} while (0);

#define PRINT_INT(var, type) \
	do { \
		var = va_arg(ap, type); \
		if (var < 0) { \
			uartputc('-'); \
			var = -var; \
		} \
		if (base == 2 || base == 16) \
			print_padded(var, base, length); \
		else \
			print_uint(var, base); \
	} while (0);

static void print_uint(uint64 num, int base);
static void print_padded(uint64 num, int base, int bytes);

static const char digits[] = "0123456789abcdef";
static struct lock printf_lk = LOCK_INITIALIZER;

static void
print_uint(uint64 num, int base)
{
	int size;
	uint64 tmp;
	char buf[64];

	if (base < 2)
		panic("invalid base (base < 2)");

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
		panic("invalid base (must be 2 or 16)");

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
printf(const char *fmt, ...)
{
	char c;
	int i;
	int invalid_length, unknown_specifier;	// for panicking correctly
	va_list ap;
	int base, length;
	char *s;

	union {
		uint8 ui8;
		uint16 ui16;
		uint32 ui32;
		uint64 ui64;
		int8 i8;
		int16 i16;
		int32 i32;
		int64 i64;
	} tmp;

	if (fmt == 0)
		panic("fmt cannot not be null");

	va_start(ap, fmt);
	c = fmt[0];
	i = 0;
	invalid_length = 0;
	unknown_specifier = 0;
	acquire(&printf_lk);
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
				// int is correct type
				PRINT_INT(tmp.i8, int);
				break;
			case 2:
				// int is correct type
				PRINT_INT(tmp.i16, int);
				break;
			case 4:
				PRINT_INT(tmp.i32, int32);
				break;
			case 8:
				PRINT_INT(tmp.i64, int64);
				break;
			default:
				invalid_length = 1;
				break;
			}
			break;
		case 'u':
			switch (length) {
			case 1:
				// unsigned int is correct type
				PRINT_UINT(tmp.ui8, unsigned int);
				break;
			case 2:
				PRINT_UINT(tmp.ui16, unsigned int);
				break;
			case 4:
				PRINT_UINT(tmp.ui32, uint32);
				break;
			case 8:
				PRINT_UINT(tmp.ui64, uint64);
				break;
			default:
				invalid_length = 1;
				break;
			}
			break;
		default:
			unknown_specifier = 1;
			break;
		}
		c = fmt[++i];
	}
	release(&printf_lk);
	va_end(ap);

	// calling printf recursively causes deadlock
	// only recurse after printf_lk was released
	if (invalid_length)
		panic("invalid length");
	if (unknown_specifier)
		panic("unknown specifier");
}
