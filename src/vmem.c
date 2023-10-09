#include "defs.h"
#include "dtb.h"
#include "memory.h"
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

void
init_vmem()
{
	pte_t *root;

	if ((root = (pte_t *)kmalloc()) == 0)
		panic("kvm: couldn't allocate root page table");

	memset(root, '\x00', PAGE_SIZE);	// make every entry invalid

	kernel_pgtable = root;
	walk(kernel_pgtable, DTB_MEMORY, 1);
}
