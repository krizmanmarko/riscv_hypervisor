#include "dtb.h"
#include "riscv_hypervisor.h"
#include "types.h"

int main();

__attribute__ ((aligned(16))) char stack0[4096 * NR_CPUS];

void
start(unsigned long hartid, void *dtb)
{
	// check support for H extension
	uint64 misa = r_misa();
	if ((misa & MISA_EXT_H) == 0) {
		// exit with fail("hypervisor extension not supported")
	}

	// Prepare for first mret
	uint64 x = r_mstatus();
	x &= ~MSTATUS_MPP;
	x |= RISCV_HS_MODE;

	// set context for mret
	w_mstatus(x);
	w_mepc((uint64) main);	// requires gcc -mcmodel=medany

	// delegate interrupts and exceptions to S
	w_medeleg(0xffff);
	w_mideleg(0xffff);

	w_satp(0);

	w_pmpaddr0(0x3fffffffffffff);
	w_pmpcfg0(0xf);


	asm volatile("mret");
}
