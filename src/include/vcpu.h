#ifndef VCPU_H
#define VCPU_H

#include "types.h"
#include "vm_config.h"

struct vcpu {
	uint64 vhartid;
	uint32 last_claimed_irq_id;		// for vplic emulation
	struct vm_config *conf;
	struct __attribute__((__packed__, aligned(sizeof(uint64)))) {
		uint64 x[32];
	} regs;
};

_Static_assert(sizeof(((struct vcpu *)0)->regs) == (32) * sizeof(uint64), "struct regs is weird");

extern struct vcpu vcpus[];

#endif // VCPU_H
