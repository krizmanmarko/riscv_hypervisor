#include "cpu.h"
#include "dtb.h"
#include "riscv_hypervisor.h"
#include "types.h"

static struct cpu cpus[DTB_NR_CPUS];
// TODO: I guess I am aligning this for performance / call convention
// beware that 4096 is also used in boot.S
__attribute__ ((aligned(16))) char cpu_stacks[4096 * DTB_NR_CPUS];

uint64 hartid();
static struct cpu *mycpu();
void interrupt_enable();
void interrupt_disable();
void push_int_disable();
void pop_int_disable();

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
