// Kernel physical memory

#include "defs.h"
#include "dtb.h"
#include "lock.h"
#include "memory.h"
#include "stdio.h"
#include "string.h"
#include "types.h"

struct ll {
	struct ll *next;
};

struct {
	struct lock lk;
	struct ll *freelist;
} kmem;

static int valid_address(void *pa);

static int
valid_address(void *pa)
{
	if (((uint64) pa % PAGE_SIZE) != 0)
		return 0;
	if ((char *)pa < end || (uint64) pa >= DTB_MEMORY + DTB_MEMORY_SIZE)
		return 0;
	return 1;
}

// returns page address on succes, 0 on fail
void *
kmalloc()
{
	struct ll *page;

	acquire(&kmem.lk);
	page = kmem.freelist;
	if (page)
		kmem.freelist = page->next;
	release(&kmem.lk);

	return (void *)page;
}

void
kfree(void *pa)
{
	struct ll *page;

	if (!valid_address(pa))
		panic("kfree");

	page = (struct ll *)pa;

	acquire(&kmem.lk);
	page->next = kmem.freelist;
	kmem.freelist = page;
	release(&kmem.lk);
}

void
init_kmem()
{
	init_lock(&kmem.lk, "kmem");
	kmem.freelist = (struct ll *)0;

	// calculate all possible page pointers in (end, max_mem]
	uint64 pa;
	pa = PGROUNDUP((uint64) end);
	while (valid_address((void *)pa)) {
		kfree((void *)pa);
		pa += PAGE_SIZE;
	}

	// last 4 bytes of memory are reserved for exception testing
	// therefore last page is thrown away forever
	if ((uint64) kmalloc() != DTB_MEMORY + DTB_MEMORY_SIZE - PAGE_SIZE)
		panic("exception testing page not discarded");
}
