// TODO: this whole file is just debugging
#include "bits.h"
#include "defs.h"
#include "riscv.h"
#include "types.h"
#include "sbi.h"
#include "stdio.h"	// TODO: look at the warning

// WARNING!
// Locks do not disable interrupts. Do not use them in interrupt handlers unless
// you know what you are doing. This also means printf() could be problematic

void
hs_interrupt_handler(uint64 scause)
{
// handling timers
// sstatus.sie
// sie.stie
// sbi_set_timer
// 	timer 1 sekunda -> 10**7

	switch (scause << 1 >> 1) {
	case INT_SUPERVISOR_TIM:
		sbi_set_timer(CSRR(time) + 10000000);
		break;
	case INT_SUPERVISOR_EXT:
		uint32 id = plic_claim(1);
		printf("HS grab\n");
		vcpus[get_hartid()].last_claimed_irq_id = id;
		CSRS(hvip, HVIP_VSEIP);
		// plic_claim happens in vplic on store
		break;
	default:
		panic("unknown interrupt %d\n", scause);
	}
}

static int
is_access_to_plic()
{
	// TODO: PLIC can be mapped anywhere, use config.device struct
	//       to check correctly
	uint64 addr = CSRR(stval);
	if (addr < DTB_PLIC || DTB_PLIC + DTB_PLIC_SIZE < addr)
		return 0;
	return 1;
}

void
hs_exception_handler(uint64 scause)
{
	switch (scause) {
	case EXC_ECALL_FROM_VS:
		//TODO: testing
		//printf("hello from exception %u\n", scause);
		CSRW(vstimecmp, CSRR(time) + 10000000);
		CSRW(sepc, CSRR(sepc) + 4);
		break;
	case EXC_LOAD_GUEST_PAGE_FAULT:
		if (is_access_to_plic())
			vplic_handle_load_page_fault();
		else
			panic("Bad read access\n");
		break;
	case EXC_STORE_OR_AMO_GUEST_PAGE_FAULT:
		if (is_access_to_plic())
			vplic_handle_store_or_amo_page_fault();
		else
			panic("Bad write access\n");
		break;
	default:
		panic("unknown exception %d\n", scause);
	}
}
