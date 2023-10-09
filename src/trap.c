#include "riscv_hypervisor.h"
#include "stdio.h"	// TODO: only for debugging with printf and panic
#include "types.h"

void
hs_interrupt_handler(uint64 scause)
{
	panic("hello from interrupt");
	scause++;
}

void
hs_exception_handler(uint64 scause)
{
	printf("hello from exception %bhhu\n", scause);
	while (1);
}
