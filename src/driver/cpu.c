#include "defs.h"
#include "dtb.h"
#include "memory.h"
#include "riscv.h"
#include "types.h"
#include "cpu.h"

extern pte_t *kernel_pgtable;

static struct cpu *mycpu();

struct cpu cpus[DTB_NR_CPUS];

uint64
hartid()
{
	uint64 id;
	asm volatile("mv %0, tp" : "=r" (id));
	return id;
}

static struct cpu *
mycpu()
{
	return &cpus[hartid()];
}

void
interrupt_enable()
{
	mycpu()->int_enable = 1;
	CSRW(sstatus, CSRR(sstatus) | SSTATUS_SIE);
}

void
interrupt_disable()
{
	mycpu()->int_enable = 0;
	CSRW(sstatus, CSRR(sstatus) & ~SSTATUS_SIE);
}

void
push_int_disable()
{
	struct cpu *cpu = mycpu();
	if (cpu->noff == 0)
		if (cpu->int_enable)
			interrupt_disable();
	cpu->noff += 1;
}

void
pop_int_disable()
{
	struct cpu *cpu = mycpu();
	cpu->noff -= 1;
	if (cpu->noff == 0)
		if (cpu->int_enable)
			interrupt_enable();
}

int
interrupt_status()
{
	return (CSRR(sstatus) & SSTATUS_SIE) ? 1 : 0;
}

void
init_hart()
{
	uint64 reg;

	// STRUCT CPU INIT (int_enable, noff, stack)

	// interrupts should be disabled at this point
	// no need to initialize cpu stack
	mycpu()->int_enable = 0;
	mycpu()->noff = 0;

	// ACTUAL HART INIT (sstatus, sie, sip, satp, stvec)
	reg = ((uint64) kernel_pgtable) >> 12;
	reg |= ATP_MODE_SV39;

	CSRW(satp, reg);
	asm volatile("sfence.vma");	// flush TLB

	reg = (uint64) hstrapvec;
	reg &= TVEC_MODE;
	reg |= TVEC_MODE_DIRECT;
	CSRW(stvec, hstrapvec);
}
