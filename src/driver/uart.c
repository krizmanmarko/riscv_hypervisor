// ns16550a compatible
// current stage: very primitive

#include "dtb.h"

#define BASE DTB_SERIAL

void
uartputc(char c)
{
	*((char *)BASE) = c;
}
