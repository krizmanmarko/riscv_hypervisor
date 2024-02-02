#ifndef VCPU_H
#define VCPU_H

#include "types.h"

struct vcpu {
	uint64 x[32 - 1];	// zero is always 0
} __attribute__((__packed__, aligned(sizeof(uint64))));

_Static_assert(sizeof(vcpu) == (31) * sizeof(uint64), "struct vcpu is weird");

#endif // VCPU_H
