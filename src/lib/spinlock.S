#include "structs_in_asm.h"

.section ".text", "ax"
.globl acquire
.globl release

# void acquire(struct lock *lk)
acquire:
	li a1, 1
1:	lw a2, (a0)			# non-atomical test
	bnez a2, 1b
	amoswap.w.aq a2, a1, (a0)	# atomic test-and-set
	bnez a2, 1b
	ret

# void release(struct lock *lk)
release:
	amoswap.w.rl zero, zero, (a0)	# no need to try
	ret
