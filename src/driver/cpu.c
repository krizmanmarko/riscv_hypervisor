#include "defs.h"
#include "dtb.h"
#include "memory.h"
#include "riscv.h"
#include "types.h"
#include "cpu.h"

__attribute__((__section__(".cpu_structs"))) struct cpu cpus[DTB_NR_CPUS];

static int cpu_mapped = 0;

struct cpu *
mycpu()
{
	uint64 id;
	if (cpu_mapped) {
		return (struct cpu *)VAS_CPU_STRUCT;
	} else {
		// hartid is stored in tp during boot
		asm volatile("mv %0, tp" : "=r" (id));
		return &cpus[id];
	}
}

uint64
get_hartid()
{
	return mycpu()->hartid;
}

void
interrupt_enable()
{
	mycpu()->int_enable = 1;
	CSRS(sstatus, SSTATUS_SIE);
}

void
interrupt_disable()
{
	mycpu()->int_enable = 0;
	CSRC(sstatus, SSTATUS_SIE);
}

void
push_int_disable()
{
	if (mycpu()->noff == 0)
		if (mycpu()->int_enable)
			interrupt_disable();
	mycpu()->noff += 1;
}

void
pop_int_disable()
{
	mycpu()->noff -= 1;
	if (mycpu()->noff == 0)
		if (mycpu()->int_enable)
			interrupt_enable();
}

int
interrupt_status()
{
	return (CSRR(sstatus) & SSTATUS_SIE) ? 1 : 0;
}

#include "stdio.h"	// TODO: delete this
void
init_hart(pte_t *pgtable)
{
	uint64 reg;
	register uint64 sp asm("sp");
	register uint64 tp asm("tp");

	// STRUCT CPU INIT
	mycpu()->hartid = tp;
	mycpu()->int_enable = 0;
	mycpu()->noff = 0;

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
	CSRW(satp, reg);
	asm volatile("sfence.vma");	// flush TLB

	// setup sp correctly
	sp -= tp * sizeof(struct cpu);
	sp -= (uint64) cpu_structs;
	sp += VAS_CPU_STRUCT;

	cpu_mapped = 1;
}
