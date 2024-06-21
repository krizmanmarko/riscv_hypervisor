#ifndef VCPU_H
#define VCPU_H

#include "types.h"

struct vcpu {
	uint64 vhartid;
	struct __attribute__((__packed__, aligned(sizeof(uint64)))) {
		uint64 x[32];
	} regs;
};

_Static_assert(sizeof(((struct vcpu *)0)->regs) == (32) * sizeof(uint64), "struct regs is weird");

#endif // VCPU_H
