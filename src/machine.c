#include "defs.h"
#include "dtb.h"
#include "riscv_hypervisor.h"

void
init_machine()
{
	// setup trap vector as early as possible
	W_MTVEC((uint64) mtrapvec);

	// check support for H extension
	uint64 misa;
	R_MISA(&misa);
	if ((misa & MISA_EXT_H) == 0) {
		while (1);
		// TODO exit with fail("hypervisor extension not supported")
		// issue is I cannot panic, since uart is not initialized
		// neither is printf so this is a weird scenario
		// Maybe move this check to later stage in code?
	}

	// setup memory
	uint64 pmpcfg0;
	int reg;

	pmpcfg0 = 0;
	reg = -1;

	// make mmio region RW
	pmpcfg0 |= (PMPCFG_A_TOR | PMPCFG_R | PMPCFG_W) << (++reg * 8);
	W_PMPADDR0(DTB_MEMORY >> 2);

	// make .boot and .text section RX
	pmpcfg0 |= (PMPCFG_A_TOR | PMPCFG_R | PMPCFG_X) << (++reg * 8);
	W_PMPADDR1(((uint64) &rodata) >> 2);

	// make .rodata section RO
	pmpcfg0 |= (PMPCFG_A_TOR | PMPCFG_R) << (++reg * 8);
	W_PMPADDR2(((uint64) &data) >> 2);

	// make .data and dynamic memory RW
	pmpcfg0 |= (PMPCFG_A_TOR | PMPCFG_R | PMPCFG_W) << (++reg * 8);
	W_PMPADDR3((DTB_MEMORY + DTB_MEMORY_SIZE) >> 2);

	// max physical addr (56-bits) -> no permissions
	pmpcfg0 |= (PMPCFG_A_TOR) << (++reg * 8);
	W_PMPADDR4((0xffffffffffffffUL) >> 2);

	W_PMPCFG0(pmpcfg0);

	// delegate exceptions (do not delegate M exceptions)
	uint64 exceptions = MEDELEG_INSTRUCTION_ADDR_MISALIGNED
		| MEDELEG_INSTRUCTION_ACCESS_FAULT
		| MEDELEG_ILLEGAL_INSTRUCTION
		| MEDELEG_BREAKPOINT
		| MEDELEG_LOAD_ADDR_MISALIGNED
		| MEDELEG_LOAD_ACCESS_FAULT
		| MEDELEG_STORE_OR_AMO_ADDRESS_MISALIGNED
		| MEDELEG_STORE_OR_AMO_ACCESS_FAULT
		| MEDELEG_ECALL_FROM_U
		| MEDELEG_ECALL_FROM_HS
		| MEDELEG_ECALL_FROM_VS
		| MEDELEG_INSTRUCTION_PAGE_FAULT
		| MEDELEG_LOAD_PAGE_FAULT
		| MEDELEG_STORE_OR_AMO_PAGE_FAULT
		| MEDELEG_INSTRUCTION_GUEST_PAGE_FAULT
		| MEDELEG_LOAD_GUEST_PAGE_FAULT
		| MEDELEG_VIRTUAL_INSTRUCTION
		| MEDELEG_STORE_OR_AMO_GUEST_PAGE_FAULT;
	W_MEDELEG(exceptions);
	// 0xf0b7ff

	// delegate interrupts (do not delegate M interrupts)
	uint64 interrupts = MIDELEG_SSI
		| MIDELEG_VSSI
		| MIDELEG_STI
		| MIDELEG_VSTI
		| MIDELEG_SEI
		| MIDELEG_VSEI
		| MIDELEG_SGEI;
	W_MIDELEG(interrupts);
	// 0x1666

	// initialize supervisor
	W_SSTATUS(0ULL);
	W_SIE(0ULL);
	W_SIP(0ULL);
	W_SATP(ATP_MODE_BARE);

	// Prepare for first mret
	uint64 mstatus;
	R_MSTATUS(&mstatus);
	mstatus &= ~MSTATUS_MPP;
	mstatus |= STATUS_MPP_S;	// S-mode
	mstatus &= ~MSTATUS_MPV;	// V = 0 -> HS-mode
	W_MSTATUS(mstatus);
	W_MEPC((uint64) main);	// requires gcc -mcmodel=medany

	// enter supervisor mode
	asm volatile("mret");
}
