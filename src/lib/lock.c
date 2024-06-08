#include "lock.h"


// SPINLOCK

void acquire(struct lock *lk);		// from spinlock.S
void release(struct lock *lk);		// from spinlock.S

void
init_lock(struct lock *lk)
{
	lk->locked = 0;
}


// BARRIER (self-resetting / eco-friendly)

// How self-resetting barrier works:
// 1. every hit to the barrier removes a token
// 2. other harts enter barrier before last hart and they are waiting for reset
// 3. last hart enters barrier and does the reset (signals other harts to cont.)
// 4. barrier is now ready for use as if it has just been initialized

// TODO: Will this haunt me in any way?
// YES -> if last hart is fast it can get stuck on next same barrier while
//        others have still not passed the first iteration of this lock
//     I added passed parameter to save myself here
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

	if (LAST_HART) {
		bar->passed = 1;
		bar->tokens = bar->initial;		// reset
		while (bar->passed != bar->initial);	// let others pass
	} else {
		while (bar->tokens != bar->initial);	// wait for reset
		acquire(&bar->lk);
		bar->passed++;				// just pass
		release(&bar->lk);
	}
}

void
init_barrier(struct barrier *bar, int count)
{
	bar->tokens = count;
	bar->initial = count;
	init_lock(&bar->lk);
}
