#ifndef LOCK_H
#define LOCK_H

#ifndef __ASSEMBLER__
	
struct lock {
	int locked;

	// debugging
	int hartid;	// hartid of cpu holding the lock
};

struct barrier {
	int count;
	struct lock lk;
};

#define LOCK_INITIALIZER \
{ \
	.locked = 0, \
	.hartid = -1 \
}

#define BARRIER_INITIALIZER(cnt) \
{ \
	.count = cnt, \
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
