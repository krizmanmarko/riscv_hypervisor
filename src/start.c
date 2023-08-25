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
		// exit with fail("hypervisor extension not supported")
	}

	// Prepare for first mret
	uint64 mstatus;
	R_MSTATUS(&mstatus);
	mstatus &= ~MSTATUS_MPP;
	mstatus |= RISCV_HS_MODE;

	// set context for mret
	W_MSTATUS(mstatus);
	W_MEPC((uint64) main);	// requires gcc -mcmodel=medany

	// delegate interrupts and exceptions to S
	W_MEDELEG(0xffff);
	W_MIDELEG(0xffff);

	W_SATP(0);

	W_PMPADDR0(0x3fffffffffffff);
	W_PMPCFG0(0xf);

	asm volatile("mret");
}
