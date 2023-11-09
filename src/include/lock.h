#ifndef LOCK_H
#define LOCK_H

// spinlock

struct lock {
	// order and types of elements must stay the same, because they are
	// hardcoded in lib/spinlock.S
	int locked;
	int hartid;	// hartid of cpu holding the lock

	// debugging
	char *name;	// purpose of the lock
};

void acquire(struct lock *lk);
void release(struct lock *lk);
void init_lock(struct lock *lk, char *name);

#endif // LOCK_H
TODO: this is an old file
