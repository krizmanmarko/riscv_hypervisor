.section ".text"
.globl acquire
.globl release
.globl init_lock

# void acquire(struct lock *lk)
# clobbers: t0, t1, store ra
acquire:
	addi sp, sp, -24
	sd ra, 16(sp)
	sd t1, 8(sp)
	sd t0, 0(sp)

	add t0, a0, zero		# a0 is caller saved
	call push_int_disable
	add a0, t0, zero
	li t0, 1
1:	lw t1, (a0)			# check if held
	bnez t1, 1b			# back to label '1' == 1b
	amoswap.w.aq t1, t0, (a0)	# try swap
	bnez t1, 1b			# back to label '1' == 1b
	fence				# seperate critical section

	ld t0, 0(sp)
	ld t1, 8(sp)
	ld ra, 16(sp)
	addi sp, sp, 24
	ret


# void release(struct lock *lk)
release:
	addi sp, sp, -8
	sd ra, 0(sp)

	fence				# seperate critical section
	amoswap.w.rl zero, zero, (a0)	# swap (no need to try)
	add t0, a0, zero		# a0 is caller saved
	call pop_int_disable
	add a0, t0, zero

	ld ra, 0(sp)
	addi sp, sp, 8
	ret


# void init_lock(struct lock *lk)
init_lock:
	sw zero, 0(a0)			# lk->locked = 0
	ret
