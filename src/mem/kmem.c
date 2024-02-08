// Kernel physical memory

#include "defs.h"
#include "dtb.h"
#include "lock.h"
#include "memory.h"
#include "stdio.h"
#include "types.h"

// Wondering what on earth this is?
// before '=': kmem variable with type of anonymous struct
// after  '=': initialization of this anonymous struct
static struct {
	struct lock lk;
	struct list *freelist;
} kmem = {
	.lk = LOCK_INITIALIZER,
	.freelist = (struct list *)0
};

static int valid_address(void *kpa);
static void bootstrap_kfree(void *kpa);

static int
valid_address(void *kpa)
{
	if (((uint64) kpa % PAGE_SIZE) != 0)
		return 0;
	if ((char *)kpa < KVA2KPA(dynamic) || (uint64) kpa >= PA2KPA(DTB_MEMORY_END))
		return 0;
	return 1;
}

// returns page address on succes, 0 on fail
void *
kmalloc()
{
	struct list *page;

	acquire(&kmem.lk);
	page = kmem.freelist;
	if (page)
		kmem.freelist = page->next;
	release(&kmem.lk);

	return (void *)page;
}

void
kfree(void *kpa)
{
	struct list *page;

	if (!valid_address(kpa))
		panic("kfree");

	page = (struct list *)kpa;

	acquire(&kmem.lk);
	page->next = kmem.freelist;
	kmem.freelist = page;
	release(&kmem.lk);
}

// It always receives valid pa
// no locking necessary since this only runs on master hart
// kfree is considerably slower (on 1GB ram: kfree: 1.8s, this: 1.2s)
static void
bootstrap_kfree(void *kpa)
{
	struct list *page;
	page = (struct list *)kpa;
	page->next = kmem.freelist;
	kmem.freelist = page;
}

void
init_kmem()
{
	// calculate all possible page pointers in (end, max_mem]
	uint64 kpa;
	kpa = KVA2KPA(PGROUNDUP((uint64) dynamic));
	while (valid_address((void *)kpa)) {
		bootstrap_kfree((void *)kpa);
		kpa += PAGE_SIZE;
	}
}
