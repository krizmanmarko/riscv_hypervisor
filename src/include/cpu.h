#ifndef CPU_H
#define CPU_H

#include "memory.h"
#include "types.h"

#define CPU_STACK_SIZE (2 * PAGE_SIZE)

#ifndef __ASSEMBLER__

// per cpu allocations
struct cpu {
	uint64 hartid;
	// stack - last used, grows towards smaller address, 16-bit aligned
	char stack[CPU_STACK_SIZE] __attribute__((aligned(PAGE_SIZE)));
};

#endif // __ASSEMBLER__
#endif // CPU_H
