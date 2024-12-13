
// %%: %
// %p: xlen_t pointer
// %s: string

#include "arch/types.h"
#include "uart.h"

void
printf(const char *fmt, ...)
{
	int len = 0;
	len = 0;
	while (fmt[len]) {
		uartputc(fmt[len]);
		len++;
	}
}
