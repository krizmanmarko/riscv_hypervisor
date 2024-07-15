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
		vplic_handle_interrupt();
		break;
	default:
		panic("unknown interrupt %d\n", scause);
	}
}

static uint64
get_pa(pte_t *pgtable)
{
	uint64 gpa = CSRR(htval) << 2;
	pte_t *pte = walk(pgtable, gpa, 0, 1);
	uint64 page_pa = PTE2PA(*pte);
	uint64 pa = page_pa | get_value(gpa, 0, 12);
	return pa;
}

static int
is_access_to_plic()
{
	// TODO: PLIC can be mapped anywhere, use config.device struct
	//       to check correctly
	uint64 addr = get_pa(get_vcpu()->conf->vm_pgtable);
	if (DTB_PLIC <= addr && addr < DTB_PLIC + DTB_PLIC_SIZE)
		return addr;
	return 0;
}


void
hs_exception_handler(uint64 scause)
{
	uint64 addr;

	switch (scause) {
	case EXC_ECALL_FROM_VS:
		//TODO: testing
		//printf("hello from exception %u\n", scause);
		CSRW(vstimecmp, CSRR(time) + 10000000);
		CSRW(sepc, CSRR(sepc) + 4);
		break;
	case EXC_LOAD_GUEST_PAGE_FAULT:
		if ((addr = is_access_to_plic()))
			vplic_handle_load_page_fault(addr);
		else
			panic("Bad read access\n");
		break;
	case EXC_STORE_OR_AMO_GUEST_PAGE_FAULT:
		if ((addr = is_access_to_plic()))
			vplic_handle_store_or_amo_page_fault(addr);
		else
			panic("Bad write access\n");
		break;
	default:
		panic("unknown exception %d\n", scause);
	}
}
