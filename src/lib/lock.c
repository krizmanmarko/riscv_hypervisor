#include "lock.h"


// SPINLOCK
// no need for lock destructor (since nothing extra is allocated)

void acquire(struct lock *lk);		// from spinlock.S
void release(struct lock *lk);		// from spinlock.S

void
init_lock(struct lock *lk)
{
	lk->locked = 0;
	lk->hartid = -1;
}


// BARRIER
// no need for barrier destructor (since nothing extra is allocated)

// How self-resetting barrier works:
// TODO: clean this comment
// A) OVERENGINEERED SOLUTION
// 1. last hart to hit the barrier lets other harts continue by setting
//      bar->tokens to 0
// 2. other harts now pass the while (they were stuck on waiting for last hart)
// 3. other harts now increment bar->tokens in order to tell the last hart that
//      they have successfully passed the while (they were stuck on waiting for
//      last hart)
// 4. when last hart finds out that all other harts have passed the while, it
//      lets all harts go (including itself)
// 5. last hart resets the barrier, and lets all harts continue

// B) SHOULD HAVE BEEN MY FIRST IDEA
// 1. every hit to the barrier removes a token
// 2. other harts enter barrier before last hart and they are waiting for reset
// 3. last hart enters barrier and does the reset (signals other harts to cont.)

#define LAST_HART (bar->tokens == 0)

void
wait_barrier(struct barrier *bar)
{
	acquire(&bar->lk);
	bar->tokens--;
	release(&bar->lk);

	if (LAST_HART)
		bar->tokens = bar->initial;		// reset
	else
		while (bar->tokens != bar->initial);	// wait for reset
}

void
init_barrier(struct barrier *bar, int count)
{
	bar->tokens = count;
	bar->initial = count;
	init_lock(&bar->lk);
}
