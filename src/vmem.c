#include "defs.h"
#include "dtb.h"
#include "memory.h"
#include "riscv_hypervisor.h"
#include "stdio.h"
#include "string.h"

pte_t *kernel_pgtable;

// returns page table entry on success or 0 on fail
// currently does not support megapages or gigapages
pte_t *
walk(pte_t *pgtable, uint64 va, int alloc)
{
	pte_t *pte;	// page table entry
	pte_t *pa;	// physical address (of page table)

	if (pgtable == 0)
		panic("root page table doesn't exist");

	pa = pgtable;
	for (int level = LEVELS - 1; level > 0; level--) {
		pte = &pa[VA2IDX(level, va)];
		if (*pte & PTE_V) {
			pa = PTE2PA(*pte);
		} else {
			if (!alloc || (pa = kmalloc()) == 0)
				return 0;
			memset(pa, '\x00', PAGE_SIZE);
			*pte = PA2PTE((uint64) pa) | PTE_V;
		}
	}
	return &pa[VA2IDX(0, va)];
}

// returns 0 on success, -1 on fail
static int
map_page(pte_t *pgtable, uint64 va, uint64 pa, int pte_flags)
{
	pte_t *leaf;

	if ((leaf = walk(pgtable, va, 1)) == 0)
		return -1;
	if (*leaf & PTE_V)
		panic("tried to remap page");
	*leaf = PA2PTE(pa) | pte_flags | PTE_V;
	return 0;
}

// va, pa and size may not be page aligned
// returns 0 on success, -1 on fail
static int
map_pages(pte_t *pgtable, uint64 va, uint64 pa, unsigned int size, int pte_flags)
{
	for (uint64 i = 0; i < PGROUNDUP(size); i += PAGE_SIZE)
		if (map_page(pgtable, va + i, pa + i, pte_flags) < 0)
			return -1;
	return 0;
}

// creates page table supervisor will actually use
void
init_vmem()
{
	int rv;
	pte_t *root;

	if ((root = (pte_t *)kmalloc()) == 0)
		panic("couldn't allocate root page table");

	memset(root, '\x00', PAGE_SIZE);	// make every entry invalid

	rv = map_page(root, DTB_FLASH, DTB_FLASH, PTE_R | PTE_W);
	rv += map_page(root, DTB_PLATFORM_BUS, DTB_PLATFORM_BUS, PTE_R | PTE_W);
	rv += map_page(root, DTB_RTC, DTB_RTC, PTE_R | PTE_W);
	rv += map_page(root, DTB_SERIAL, DTB_SERIAL, PTE_R | PTE_W);
	rv += map_page(root, DTB_CLINT, DTB_CLINT, PTE_R | PTE_W);
	if (rv < 0)
		panic("failed to map MMIO");

	// map kernel image
	uint64 va = (uint64) text;
	uint64 pa = VA2PA((uint64) text);
	unsigned int size = (unsigned int) (etext - text);
	rv = map_pages(root, va, pa, size, PTE_R | PTE_X);

	va = (uint64) rodata;
	pa = VA2PA((uint64) rodata);
	size = (unsigned int) (erodata - rodata);
	rv = map_pages(root, va, pa, size, PTE_R);

	va = (uint64) data;
	pa = VA2PA((uint64) data);
	size = (unsigned int) (edata - data);
	rv = map_pages(root, va, pa, size, PTE_R | PTE_W);

	asm volatile("sfence.vma");
	uint64 ppn = ((uint64) root) >> 12;
	W_SATP(ATP_MODE_Sv39 | ppn);

	kernel_pgtable = root;
}
