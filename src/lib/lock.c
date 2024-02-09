#include "lock.h"


// SPINLOCK
// no need for lock destructor (since nothing extra is allocated)

void acquire(struct lock *lk);		// from spinlock.S
void release(struct lock *lk);		// from spinlock.S

void
init_lock(struct lock *lk)
{
	lk->locked = 0;
}


// BARRIER (self-resetting / eco-friendly)
// no need for barrier destructor (since nothing extra is allocated)

// How self-resetting barrier works:
// 1. every hit to the barrier removes a token
// 2. other harts enter barrier before last hart and they are waiting for reset
// 3. last hart enters barrier and does the reset (signals other harts to cont.)
// 4. barrier is now ready for use as if it has just been initialized

// TODO: Will this haunt me in any way?
// why have self-resetting barrier
//   it is annoying to allocate many barriers (for example) in main to just
//   sync same harts at many points. Now I can use the same barrier
// It does not hurt:
// tiny overhead compared to classic barrier (you are using spinchecks afterall,
// extra 'if' does no harm), plus you can always use them as normal barriers

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
