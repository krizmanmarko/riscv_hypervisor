#ifndef VCPU_H
#define VCPU_H

#include "types.h"

struct vcpu {
	uint64 x[32];	// zero is always 0 (need only 31 spaces, but indexing)
} __attribute__((__packed__, aligned(sizeof(uint64))));

_Static_assert(sizeof(struct vcpu) == (32) * sizeof(uint64), "struct vcpu is weird");

#endif // VCPU_H
