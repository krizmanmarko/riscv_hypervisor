#include "dtb.h"
#include "riscv_hypervisor.h"
#include "types.h"

int main();

__attribute__ ((aligned(16))) char stack0[4096 * DTB_NR_CPUS];

void
start(unsigned long hartid, void *dtb)
{
	// check support for H extension
	uint64 misa;
	R_MISA(&misa);
	if ((misa & MISA_EXT_H) == 0) {
		while (1);
		// TODO exit with fail("hypervisor extension not supported")
	}

	// setup memory
	W_PMPCFG0(PMPCFG_A_TOR | PMPCFG_R | PMPCFG_W | PMPCFG_X);
	W_PMPADDR0(0x3fffffffffffff);
	W_SATP(ATP_MODE_BARE);

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
		| MEDELEG_ECALL_FROM_S
		| MEDELEG_INSTRUCTION_PAGE_FAULT
		| MEDELEG_STORE_OR_AMO_PAGE_FAULT;
	W_MEDELEG(exceptions);

	// delegate interrupts (do not delegate M interrupts)
	uint64 interrupts = INT_HSSI
		| INT_VSSI
		| INT_HSTI
		| INT_VSTI
		| INT_HSEI
		| INT_VSEI
		| INT_SGEI;
	W_MIDELEG(interrupts);

	// Prepare for first mret
	uint64 mstatus;
	R_MSTATUS(&mstatus);
	mstatus &= ~MSTATUS_MPP;
	mstatus |= STATUS_MPP_S;	// S-mode
	mstatus &= ~MSTATUS_MPV;	// V = 0 -> HS-mode
	W_MSTATUS(mstatus);
	W_MEPC((uint64) main);	// requires gcc -mcmodel=medany

	// Testing purposes
	W_MTVEC((uint64) 0x1000);
	W_STVEC((uint64) 0x2000);

	asm volatile("mret");
}
