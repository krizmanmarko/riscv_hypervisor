#include "riscv.h"
#include "types.h"
#include "stdio.h"	// TODO: only for debugging with printf and panic
#include "sbi.h"	// TODO: only for debugging

// handling timers
// sstatus.sie
// sie.stie
// sbi_set_timer
// 	timer 1 sekunda -> 10**7

// watch out for this pending bit issue
// https://github.com/riscv-software-src/opensbi/issues/154

void
hs_interrupt_handler(uint64 scause)
{
	printf("hello from interrupt %u\n", scause);
	sbi_set_timer(CSRR(time) + 10000000);
}

void
hs_exception_handler(uint64 scause)
{
	if (scause == EXC_ECALL_FROM_VS) {
		printf("hello from exception %u\n", scause);
		CSRW(vstimecmp, CSRR(time) + 10000000);
		CSRW(sepc, CSRR(sepc) + 4);
	}
}
