#include "dtb.h"
#include "memory.h"
#include "riscv_hypervisor.h"
#include "types.h"
#include "cpu.h"

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
	uint64 sstatus;
	R_SSTATUS(&sstatus);
	mycpu()->int_enable = 1;
	W_SSTATUS(sstatus | SSTATUS_SIE);
}

void
interrupt_disable()
{
	uint64 sstatus;
	R_SSTATUS(&sstatus);
	mycpu()->int_enable = 0;
	W_SSTATUS(sstatus & ~SSTATUS_SIE);
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

int interrupt_status()
{
	uint64 sstatus;
	R_SSTATUS(&sstatus);
	return (sstatus & SSTATUS_SIE) ? 1 : 0;
}

void hart_init_cpu()
{
	// no need to initialize cpu stack
	interrupt_disable();
	mycpu()->int_enable = 0;
	mycpu()->noff = 0;
}
