#ifndef CPU_H
#define CPU_H

#include "memory.h"
#include "types.h"

#ifndef __ASSEMBLER__

// per cpu allocations (WARNING: stack must be last, for easier sp handling)
struct cpu {
	uint64 hartid;
	// stack - last used, grows towards smaller address, 16-bit aligned
	char stack[2 * PAGE_SIZE] __attribute__((aligned(PAGE_SIZE)));
};

#endif // __ASSEMBLER__
#endif // CPU_H
