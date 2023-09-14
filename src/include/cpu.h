#ifndef CPU_H
#define CPU_H

// per cpu allocations
struct cpu {
	// stack
	// defined in cpu.c for all dtb cpus (but should belong here)
	//   if we declared it in struct cpu, we do not know the offset to
	//   this field in assembly code (boot.S), position may even change
	//   in the future

	// interrupts
	int int_enable;	// stored for push and pop
	int noff;	// times interrupt_disable was pushed but not popped
};

#endif // CPU_H
