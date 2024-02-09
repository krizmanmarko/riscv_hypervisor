#ifndef LOCK_H
#define LOCK_H

#ifndef __ASSEMBLER__

// TODO: should this be typedef if there is only one element???
struct lock {
	int locked;
};

struct barrier {
	int tokens;	// number of harts we are still waiting for
	int initial;	// initial token count
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
