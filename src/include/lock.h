#ifndef LOCK_H
#define LOCK_H

#ifndef __ASSEMBLER__

// TODO: why is this a structure? - maybe to hide volatile
struct lock {
	volatile int locked;
};

struct barrier {
	int initial;	// initial token count
	volatile int tokens;	// number of harts we are still waiting for
	volatile int passed;	// number of harts already that passed through
	struct lock lk;
};

#define LOCK_INITIALIZER \
{ \
	.locked = 0 \
}

#define BARRIER_INITIALIZER(cnt) \
{ \
	.tokens = cnt, \
	.initial = cnt, \
	.lk = LOCK_INITIALIZER \
}

#endif // __ASSEMBLER__

// spinlock
void acquire(struct lock *lk);
void release(struct lock *lk);
void init_lock(struct lock *lk);

// barrier
void wait_barrier(struct barrier *bar);
void init_barrier(struct barrier *bar, int count);

#endif // LOCK_H
