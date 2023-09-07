#include "riscv_hypervisor.h"

int uartputc(char c);
int printf(char *fmt, ...);
void test();

int
main()
{
	test();
	while (1);
}
