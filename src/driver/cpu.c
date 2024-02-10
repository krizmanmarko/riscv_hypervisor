#include "defs.h"
#include "dtb.h"
#include "memory.h"
#include "riscv.h"
#include "types.h"
#include "cpu.h"

__attribute__((__section__(".cpu_structs"))) struct cpu cpus[DTB_NR_CPUS];

int cpu_mapped = 0;

struct cpu *
mycpu()
{
	register uint64 tp __asm__("tp");	// hartid is in tp during boot
	if (cpu_mapped)
		return (struct cpu *)VAS_CPU_STRUCT;
	else
		return &cpus[tp];
}

uint64
get_hartid()
{
	return mycpu()->hartid;
}

void
init_hart(pte_t *pgtable)
{
	uint64 reg;
	register uint64 a0 __asm__("a0");
	register uint64 tp __asm__("tp");

	// STRUCT CPU INIT
	mycpu()->hartid = tp;

	// ACTUAL HART INIT (sstatus, sie, sip, satp, stvec)
	CSRS(sstatus, SSTATUS_SIE);
	CSRC(sie, SIE_STIE);
	CSRS(hie, HIE_VSTIE);

	reg = (uint64) hstrapvec;
	reg &= TVEC_MODE;
	reg |= TVEC_MODE_DIRECT;
	CSRW(stvec, hstrapvec);

	reg = (KPA2PA((uint64) pgtable)) >> 12;
	reg |= ATP_MODE_SV39;

	// BEGIN WARNING: super delicate code section
	CSRW(satp, reg);
	__asm__ volatile("sfence.vma");	// flush TLB
	// code here must not use sp/fp relative addresses
	a0 = VAS_CPU_STRUCT;
	a0 -= tp * sizeof(struct cpu);
	a0 -= (uint64) cpu_structs;
	relocate_stack(a0);
	// END OF WARNING: super delicate code section

	return;
}
