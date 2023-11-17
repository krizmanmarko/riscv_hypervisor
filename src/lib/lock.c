#include "lock.h"


// SPINLOCK
// no need for lock destructor (since nothing extra is allocated)

void acquire(struct lock *lk);		// from spinlock.S
void release(struct lock *lk);		// from spinlock.S

/*
void
init_lock(struct lock *lk)
{
	lk->locked = 0;
	lk->hartid = -1;
}
*/


// BARRIER
// so far I noticed an issue
//	-> barrier is usable only once
//	-> to initialize a barrier i have to create a barrier by hand
// it works but seems quite pointless
//
// maybe create barrier that can be used multiple times without reinitialization

// no need for barrier destructor (since nothing extra is allocated)

void
wait_barrier(struct barrier *bar)
{
	acquire(&bar->lk);
	bar->count -= 1;
	release(&bar->lk);
	while (bar->count > 0);
}

/*
void
init_barrier(struct barrier *bar, int count)
{
	bar->count = count;
	init_lock(&bar->lk);
}
*/
