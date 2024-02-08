#ifndef CPU_H
#define CPU_H

#include "memory.h"
#include "types.h"

#define CPU_STACK_SIZE (2 * PAGE_SIZE)

#ifndef __ASSEMBLER__

// per cpu allocations
struct cpu {
	// interrupts
	uint64 hartid;
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
