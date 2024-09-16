#include "bits.h"
#include "defs.h"
#include "dtb.h"
#include "cpu.h"
#include "memory.h"
#include "riscv.h"
#include "stdio.h"
#include "string.h"

static uint64
va2idx(uint64 va, int level, int x4)
{
	if (level == LEVELS - 1 && x4 != 0)	// root page table (sv39x4)
			return get_value(va, level * 9 + PAGE_SHIFT, 11);
	return get_value(va, level * 9 + PAGE_SHIFT, 9);
}

// returns page table entry on success or 0 on fail
// currently does not support megapages or gigapages
// if sv39x4 pgtable: x4 = true
pte_t *
walk(pte_t *pgtable, uint64 va, int alloc, int x4)
{
	pte_t *pte;	// page table entry
	pte_t *kpa;	// not actual physical address (of page table)

	if (pgtable == 0)
		panic("root page table doesn't exist");

	kpa = pgtable;
	for (int level = LEVELS - 1; level > 0; level--) {
		pte = &kpa[va2idx(va, level, x4)];
		if (*pte & PTE_V) {
			kpa = (pte_t *)PA2KPA(PTE2PA(*pte));
		} else {
			if (!alloc || (kpa = kmalloc()) == 0)
				return 0;
			memset(kpa, 0x00, PAGE_SIZE);
			*pte = PA2PTE((uint64) KPA2PA((uint64) kpa)) | PTE_V;
		}
	}
	return &kpa[va2idx(va, 0, x4)];
}

// returns 0 on success, -1 on fail
int
map_page(pte_t *pgtable, uint64 va, uint64 pa, int pte_flags, int x4)
{
	pte_t *leaf;

	if ((leaf = walk(pgtable, va, 1, x4)) == 0)
		return -1;
	if (*leaf & PTE_V)
		panic("tried to remap page");
	*leaf = PA2PTE(pa) | pte_flags | PTE_V;
	return 0;
}

// va, pa and size may not be page aligned
// returns 0 on success, -1 on fail
int
map_pages(pte_t *pgtable, uint64 va, uint64 pa, uint64 size, int pte_flags, int x4)
{
	uint64 tmp;
	for (uint64 i = 0; i < PGROUNDUP(size); i += PAGE_SIZE) {
		if (pa == 0) {
			tmp = KPA2PA((uint64) kmalloc());
		} else {
			tmp = pa + i;
		}
		if (map_page(pgtable, va + i, tmp, pte_flags, x4) < 0) {
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
	memset(root, 0, PAGE_SIZE);

	// map MMIO
	rv = map_page(root, DTB_FLASH, DTB_FLASH, PTE_R | PTE_W | PTE_G, 0);
	rv += map_page(root, DTB_PLATFORM_BUS, DTB_PLATFORM_BUS, PTE_R | PTE_W | PTE_G, 0);
	rv += map_page(root, DTB_RTC, DTB_RTC, PTE_R | PTE_W | PTE_G, 0);
	rv += map_page(root, DTB_SERIAL, DTB_SERIAL, PTE_R | PTE_W | PTE_G, 0);
	rv += map_pages(root, DTB_PCI, DTB_PCI, DTB_PCI_SIZE, PTE_R | PTE_W | PTE_G, 0);
	rv += map_pages(root, DTB_VIRTIO0, DTB_VIRTIO0, DTB_VIRTIO_SIZE, PTE_R | PTE_W | PTE_G, 0);
	rv += map_pages(root, DTB_PLIC, DTB_PLIC, DTB_PLIC_SIZE, PTE_R | PTE_W | PTE_G, 0);
	rv += map_pages(root, DTB_CLINT, DTB_CLINT, DTB_CLINT_SIZE, PTE_R | PTE_W | PTE_G, 0);
	if (rv < 0)
		panic("failed to map %s", "MMIO");

	// map kernel image
	va = (uint64) text;
	pa = KVA2PA((uint64) text);
	size = (unsigned int) (etext - text);
	if (map_pages(root, va, pa, size, PTE_R | PTE_X | PTE_G, 0))
		panic("failed to map %s", ".text");

	va = (uint64) rodata;
	pa = KVA2PA((uint64) rodata);
	size = (unsigned int) (erodata - rodata);
	if (map_pages(root, va, pa, size, PTE_R | PTE_G, 0))
		panic("failed to map %s", ".rodata");

	va = (uint64) data;
	pa = KVA2PA((uint64) data);
	size = (unsigned int) (edata - data);
	if (map_pages(root, va, pa, size, PTE_R | PTE_W | PTE_G, 0))
		panic("failed to map %s", ".data");

	// map stack
	va = VAS_CPU_STRUCT;
	pa = KVA2PA((uint64) mycpu());
	size = sizeof(struct cpu);
	if (map_pages(root, va, pa, size, PTE_R | PTE_W, 0))
		panic("failed to map %s", "cpu struct");

	// map whole RAM (except kernel image)
	va = KVA2KPA((uint64) dynamic);
	pa = KVA2PA((uint64) dynamic);
	size = DTB_MEMORY_SIZE - (pa - DTB_MEMORY);
	if (map_pages(root, va, pa, size, PTE_R | PTE_W | PTE_G, 0))
		panic("failed to map %s", "RAM");

	return root;
}

uint64
convert_va_to_pa(pte_t *pgtable, uint64 va, int x4)
{
	pte_t *pte = walk(pgtable, va, 0, x4);
	if (pte == 0)
		panic("bad walk for va: %p", va);
	uint64 page_pa = PTE2PA(*pte);
	uint64 pa = page_pa | get_value(va, 0, 12);
	return pa;
}
