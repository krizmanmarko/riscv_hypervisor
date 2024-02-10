#include "riscv.h"
#include "types.h"
#include "sbi.h"
#include "stdio.h"	// TODO: only for debugging with printf and panic

// WARNING!
// Locks do not disable interrupts. Do not use them in interrupt handlers unless
// you know what you are doing. This also means no printf()

void
hs_interrupt_handler(uint64 scause)
{
// handling timers
// sstatus.sie
// sie.stie
// sbi_set_timer
// 	timer 1 sekunda -> 10**7

	if (scause == INT_STI)
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
