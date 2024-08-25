#ifndef VCPU_H
#define VCPU_H

#include "types.h"
#include "vm_config.h"

struct vcpu {
	uint64 vhartid;
	// TODO:
	// this can be raced -> irq happens before vplic handles current irq
	// probably easiest to create linked list and just add on irq
	uint32 last_claimed_irq_id;		// vplic emulation (phys_irq)
	struct vm_config *conf;
	struct __attribute__((__packed__, aligned(sizeof(uint64)))) {
		uint64 x[32];
	} regs;
};

_Static_assert(sizeof(((struct vcpu *)0)->regs) == (32) * sizeof(uint64), "struct regs is weird");

extern struct vcpu vcpus[];

#endif // VCPU_H
