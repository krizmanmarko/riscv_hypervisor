#include "defs.h"
#include "dtb.h"
#include "cpu.h"
#include "memory.h"
#include "riscv.h"
#include "stdio.h"
#include "string.h"

static int map_gigapage(pte_t *pgtable, uint64 va, uint64 pa, int pte_flags);

// returns page table entry on success or 0 on fail
// currently does not support megapages or gigapages
pte_t *
walk(pte_t *pgtable, uint64 va, int alloc)
{
	pte_t *pte;	// page table entry
	pte_t *kpa;	// not actual physical address (of page table)

	if (pgtable == 0)
		panic("root page table doesn't exist");

	kpa = pgtable;
	for (int level = LEVELS - 1; level > 0; level--) {
		pte = &kpa[VA2IDX(level, va)];
		if (*pte & PTE_V) {
			kpa = (pte_t *)PA2KPA(PTE2PA(*pte));
		} else {
			if (!alloc || (kpa = kmalloc()) == 0)
				return 0;
			memset(kpa, '\x00', PAGE_SIZE);
			*pte = PA2PTE((uint64) KPA2PA((uint64) kpa)) | PTE_V;
		}
	}
	return &kpa[VA2IDX(0, va)];
}

// returns 0 on success, -1 on fail
int
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

static int
map_gigapage(pte_t *pgtable, uint64 va, uint64 pa, int pte_flags)
{
	if (pgtable == 0)
		panic("root page table doesn't exist");

	// check alignment (both va and pa must be aligned)
	if (((va | pa) & 0x3fffffff) != 0)
		return -1;
	pgtable[VA2IDX(2, va)] = PA2PTE(pa) | pte_flags | PTE_V;
	return 0;
}

// va, pa and size may not be page aligned
// returns 0 on success, -1 on fail
int
map_pages(pte_t *pgtable, uint64 va, uint64 pa, uint64 size, int pte_flags)
{
	// 0x40000000 -> 1GB
	while (size >= 0x40000000) {
		if (map_gigapage(pgtable, va, pa, pte_flags) < 0)
			return -1;
		va += 0x40000000;
		pa += 0x40000000;
		size -= 0x40000000;
	}
	for (uint64 i = 0; i < PGROUNDUP(size); i += PAGE_SIZE) {
		if (map_page(pgtable, va + i, pa + i, pte_flags) < 0) {
			return -1;
		}
	}
	return 0;
}

pte_t *
init_vmem()
{
	pte_t *root;
	int rv, size;
	uint64 va, pa;

	if ((root = (pte_t *)kmalloc()) == 0)
		panic("couldn't allocate root page table");
	memset(root, '\x00', PAGE_SIZE);

	// map MMIO
	rv = map_page(root, DTB_FLASH, DTB_FLASH, PTE_R | PTE_W | PTE_G);
	rv += map_page(root, DTB_PLATFORM_BUS, DTB_PLATFORM_BUS, PTE_R | PTE_W | PTE_G);
	rv += map_page(root, DTB_RTC, DTB_RTC, PTE_R | PTE_W | PTE_G);
	rv += map_page(root, DTB_SERIAL, DTB_SERIAL, PTE_R | PTE_W | PTE_G);
	rv += map_pages(root, DTB_PCI, DTB_PCI, DTB_PCI_SIZE, PTE_R | PTE_W | PTE_G);
	rv += map_pages(root, DTB_PLIC, DTB_PLIC, DTB_PLIC_SIZE, PTE_R | PTE_W | PTE_G);
	rv += map_pages(root, DTB_CLINT, DTB_CLINT, DTB_CLINT_SIZE, PTE_R | PTE_W | PTE_G);
	if (rv < 0)
		panic("failed to map %s", "MMIO");

	// map kernel image
	va = (uint64) text;
	pa = KVA2PA((uint64) text);
	size = (unsigned int) (etext - text);
	if (map_pages(root, va, pa, size, PTE_R | PTE_X | PTE_G))
		panic("failed to map %s", ".text");

	va = (uint64) rodata;
	pa = KVA2PA((uint64) rodata);
	size = (unsigned int) (erodata - rodata);
	if (map_pages(root, va, pa, size, PTE_R | PTE_G))
		panic("failed to map %s", ".rodata");

	va = (uint64) data;
	pa = KVA2PA((uint64) data);
	size = (unsigned int) (edata - data);
	if (map_pages(root, va, pa, size, PTE_R | PTE_W | PTE_G))
		panic("failed to map %s", ".data");

	// map stack
	va = VAS_CPU_STRUCT;
	pa = KVA2PA((uint64) mycpu());
	size = sizeof(struct cpu);
	if (map_pages(root, va, pa, size, PTE_R | PTE_W))
		panic("failed to map %s", "cpu struct");

	// map whole RAM
	va = VAS_RAM;
	pa = DTB_MEMORY;
	size = DTB_MEMORY_SIZE;
	if (map_pages(root, va, pa, size, PTE_R | PTE_W | PTE_G))
		panic("failed to map %s", "RAM");

	return root;
}
