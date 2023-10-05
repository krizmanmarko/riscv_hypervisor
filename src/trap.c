#include "riscv_hypervisor.h"
#include "types.h"

void printf(char *fmt, ...);


void
hs_interrupt_handler(uint64 scause)
{
#include "panic.h"
	panic("hello from interrupt");
}

void
hs_exception_handler(uint64 scause)
{
	printf("hello from exception %bhhu\n", scause);
	while (1);
}
