#ifndef CPU_H
#define CPU_H

#include "memory.h"

#define CPU_STACK_SIZE PAGE_SIZE

#ifndef __ASSEMBLER__

// per cpu allocations (mapped to VAS_CPU_BASE)
struct cpu {
	// interrupts
	int int_enable;	// stored for push and pop
	int noff;	// times interrupt_disable was pushed but not popped

	// stack
	// last used, grows towards smaller address, 16-bit aligned
	//	0xfff0 -> aligned
	//	0xfff8 -> not aligned
	char stack[CPU_STACK_SIZE] __attribute__((aligned(PAGE_SIZE)));
};

#endif // __ASSEMBLER__
#endif // CPU_H
