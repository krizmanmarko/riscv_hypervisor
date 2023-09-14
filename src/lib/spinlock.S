.section ".text"
.globl acquire
.globl release
.globl init_lock

# void acquire(struct lock *lk)
acquire:
	addi sp, sp, -24
	sd ra, 16(sp)
	sd t1, 8(sp)
	sd t0, 0(sp)

	mv t0, a0			# a0 is caller saved
	call push_int_disable
	mv a0, t0

	# lock->locked = 1
	li t0, 1
1:	amoswap.w.aq t1, t0, (a0)	# try swap
	bnez t1, 1b			# back to label '1' == 1b
	fence				# TODO: is this truly necessary

	# lock->hartid = hartid
	mv t0, a0			# a0 is caller saved
	call hartid
	sw a0, 4(t0)

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
	sw t0, 4(a0)

	fence				# TODO: is this truly necessary
	amoswap.w.rl zero, zero, (a0)	# swap (no need to try)
	call pop_int_disable

	ld t0, 0(sp)
	ld ra, 8(sp)
	addi sp, sp, 16
	ret
