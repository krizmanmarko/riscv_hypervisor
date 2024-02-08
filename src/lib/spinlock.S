# TODO: why are interrupts disabled for the whole duration
#	of the critical section? maybe reenable them after
#	acquiring the lock?

#include "structs_in_asm.h"

.section ".text", "ax"
.globl acquire
.globl release

# void acquire(struct lock *lk)
acquire:
	addi sp, sp, -24
	sd ra, 16(sp)
	sd t1, 8(sp)
	sd t0, 0(sp)

	mv t0, a0			# a0 is caller saved
	call push_int_disable
	mv a0, t0			# restore a0

	# lock->locked = 1
	addi a0, a0, LOCK_LOCKED_OFF
	li t0, 1
1:	amoswap.w.aq t1, t0, (a0)	# try swap
	bnez t1, 1b			# back to label '1' == 1b

	# lock->hartid = hartid
	mv t0, a0
	call get_hartid
	sw a0, LOCK_HARTID_OFF(t0)

	ld t0, 0(sp)
	ld t1, 8(sp)
	ld ra, 16(sp)
	addi sp, sp, 24
	ret


# void release(struct lock *lk)
release:
	addi sp, sp, -16
	sd ra, 8(sp)
	sd t0, 0(sp)

	# lock->hartid = -1
	li t0, -1
	sw t0, LOCK_HARTID_OFF(a0)

	# swap (no need to try)
	addi a0, a0, LOCK_LOCKED_OFF
	amoswap.w.rl zero, zero, (a0)
	call pop_int_disable

	ld t0, 0(sp)
	ld ra, 8(sp)
	addi sp, sp, 16
	ret
