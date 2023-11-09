#include "lock.h"

void init_lock(struct lock *lk, char *name)
{
	lk->locked = 0;
	lk->hartid = -1;
	lk->name = name;
}
TODO: this is an old file
