#include "dtb.h"
#include "riscv_hypervisor.h"

extern void mtrapvec();
extern void strapvec();
extern int main();

void start(unsigned long hartid, void *dtb);

extern char *etext;	// linker.ld

void
start(unsigned long hartid, void *dtb)
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
	}

	// setup memory
	uint64 pmpcfg0;

	// Leave 16 bytes for access faults exception tests
	pmpcfg0 = PMPCFG_A_TOR | PMPCFG_R | PMPCFG_W | PMPCFG_X;
	W_PMPADDR0((DTB_MEMORY + DTB_MEMORY_SIZE - 8) >> 2);
	pmpcfg0 |= (PMPCFG_A_TOR) << 8;
	W_PMPADDR1(0x3fffffffffffffUL);
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

	// delegate interrupts (do not delegate M interrupts)
	uint64 interrupts = MIDELEG_SSI
		| MIDELEG_VSSI
		| MIDELEG_STI
		| MIDELEG_VSTI
		| MIDELEG_SEI
		| MIDELEG_VSEI
		| MIDELEG_SGEI;
	W_MIDELEG(interrupts);

	// initialize supervisor
	W_SSTATUS((uint64) 0);
	W_SIE((uint64) 0);
	W_SIP((uint64) 0);
	W_SATP(ATP_MODE_BARE);
	W_STVEC((uint64) strapvec);
	//W_SSCRATCH();

	// Prepare for first mret
	uint64 mstatus;
	R_MSTATUS(&mstatus);
	mstatus &= ~MSTATUS_MPP;
	mstatus |= STATUS_MPP_S;	// S-mode
	mstatus &= ~MSTATUS_MPV;	// V = 0 -> HS-mode
	W_MSTATUS(mstatus);
	W_MEPC((uint64) main);	// requires gcc -mcmodel=medany

	// Testing purposes
	// end Testing purposes

	// enter supervisor mode
	asm volatile("mret");
}
