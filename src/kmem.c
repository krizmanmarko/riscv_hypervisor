// Kernel physical memory

/*

https://forum.osdev.org/viewtopic.php?t=12022

In general there's 3 completely different "levels".

At the lowest level is the "physical memory manager", which keeps track of free physical pages of RAM and nothing else. It only ever works on multiples of 4 KB (it can give you 4 KB of RAM or 8 KB of RAM, but not 100 bytes), and doesn't really care what the RAM is used for. Often this is built into the kernel. For a very simple system the physical memory manager might have 2 functions that are never used directly (e.g. by applications, etc):

    void *allocPhysPage();
    void freePhysPage(void *page);

On top of this there's a "linear memory manager" or "virtual memory manager", which uses the physical memory manager to allocate and free physical pages of RAM. It uses these pages to create and maintain page directories, page tables, etc, and also handles things like swap space, memory mapped files and other "tricks" (shared memory, allocation on demand, etc). It also only ever works on multiples of 4 KB. Often this is built into the kernel. For a very simple system the linear/virtual memory manager might have 2 functions that applications, etc can use:

    int allocPage(void *address);
    void freePage(void *address);

In addition, it'd provide some functions intended for the rest of the kernel to use:

    int createAddressSpace(void);
    int destroyAddressSpace(void *addressSpace);

Lastly there's something to manage which parts of an address space are being used for what. This is the "heap manager" (and might include "objstacks", garbage collection, etc). The heap manager allows you to ask for any number of bytes of space and free it later. Often this isn't built into the kernel, but is implemented in a high level language library (e.g. malloc(), free(), new(), delete(), etc). This allows different applications to have completely different heap management code and reduces the number of times the application needs to call the kernel.

But why have physical memory manager???
I guess I will handle physical pages with physical addresses with phys memory manager
it also holds full freelist (which holds physical addresses)
any other way is just confusing
paddr_t kalloc();
void kfree(paddr_t *page);

virtual memory manager will be used to map physpages to correct location in vmem
int alloc(vaddr_t *address);
void free(vaddr_t *address);
/*

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
TODO: this is an old file
