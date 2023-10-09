#include "dtb.h"
#include "memory.h"
#include "riscv_hypervisor.h"
#include "types.h"

// BEWARE: 4096 is hardcoded in boot.S for CPU_STACK_SIZE
#define CPU_STACK_SIZE PAGE_SIZE

// per cpu allocations
struct cpu {
	// stack
	// defined as cpu_stacks for all dtb cpus (but should belong here)
	//   if we declared it in struct cpu, we do not know the offset to
	//   this field in assembly code (boot.S), position may even change
	//   in the future

	// interrupts
	int int_enable;	// stored for push and pop
	int noff;	// times interrupt_disable was pushed but not popped
};

static struct cpu *mycpu();

// Call convention demands alignment of stack pointer to 16.
//	0xfff0 -> aligned
//	0xfff8 -> not aligned
// It also increases performance:
//	ld t0, 0(sp) -> only requires one fetch in DRAM
//	ld t0, 1(sp) -> requires 0(sp) and 8(sp) fetches
// TODO: At least I think this is what happens (not the goal of my research)
__attribute__ ((aligned(16))) char cpu_stacks[CPU_STACK_SIZE * DTB_NR_CPUS];
static struct cpu cpus[DTB_NR_CPUS];

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

void init_cpu()
{
	// no need to initialize cpu stack
	interrupt_disable();
	mycpu()->int_enable = 0;
	mycpu()->noff = 0;
}
