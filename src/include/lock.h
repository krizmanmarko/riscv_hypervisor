#ifndef LOCK_H
#define LOCK_H

// spinlock

#ifndef __ASSEMBLER__
struct lock {
	int locked;

	// debugging
	int hartid;	// hartid of cpu holding the lock
	char *name;	// purpose of the lock
};

#endif // __ASSEMBLER__

void acquire(struct lock *lk);
void release(struct lock *lk);
void init_lock(struct lock *lk, char *name);

#endif // LOCK_H
