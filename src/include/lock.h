// spinlock
struct lock {
	int locked;
};

extern void acquire(struct lock *lk);
extern void release(struct lock *lk);
extern void init_lock(struct lock *lk);
