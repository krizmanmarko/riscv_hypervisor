//#include "defs.h"
//#include "dtb.h"
//#include "memory.h"
//#include "riscv_hypervisor.h"
//#include "stdio.h"
//#include "string.h"
//
//pte_t *kernel_pgtable;
//
//// returns page table entry on success or 0 on fail
//// currently does not support megapages or gigapages
//pte_t *
//walk(pte_t *pgtable, uint64 va, int alloc)
//{
//	pte_t *pte;	// page table entry
//	pte_t *pa;	// physical address (of page table)
//
//	if (pgtable == 0)
//		panic("root page table doesn't exist");
//
//	pa = pgtable;
//	for (int level = LEVELS - 1; level > 0; level--) {
//		pte = &pa[VA2IDX(level, va)];
//		if (*pte & PTE_V) {
//			pa = PTE2PA(*pte);
//		} else {
//			if (!alloc || (pa = kmalloc()) == 0)
//				return 0;
//			memset(pa, '\x00', PAGE_SIZE);
//			*pte = PA2PTE((uint64) pa) | PTE_V;
//		}
//	}
//	return &pa[VA2IDX(0, va)];
//}
//
//static int
//map_page(pte_t *pgtable, uint64 va, uint64 pa, int pte_flags)
//{
//	pte_t *leaf;
//
//	if ((leaf = walk(pgtable, va, 1)) == 0)
//		return -1;
//	if (*leaf & PTE_V)
//		panic("tried to remap page");
//	*leaf = PA2PTE(pa) | pte_flags | PTE_V;
//	return 0;
//}
//
//// va, pa and size may not be page aligned
//// returns 0 on success, -1 on fail
//static int
//map_pages(pte_t *pgtable, uint64 va, uint64 pa, unsigned int size, int pte_flags)
//{
//	for (uint64 i = 0; i < PGROUNDUP(size); i += PAGE_SIZE)
//		if (map_page(pgtable, va + i, pa + i, pte_flags) < 0)
//			return -1;
//	return 0;
//}

#include "defs.h"
#include "dtb.h"
#include "memory.h"
#include "stdio.h"
#include "types.h"

// just maps serial into bootstrap pages manually for early printfs

//void
//init_bootstrap_serial()
//{
//	uint64 root = PA2VA((uint64) bsp_root);
//	pte_t *l1;
//	pte_t *l0;
//	pte_t *page;
//
//	l1 = kmalloc();
//	l0 = kmalloc();
//	page = kmalloc();
//	if (l1 == 0 || l0 == 0 || page == 0)
//		while (1);	// kmalloc failed
//	bsp_root[VA2IDX(2, DTB_SERIAL)] = l1;
//	l1[VA2IDX(1, DTB_SERIAL)] = l0;
//	l0[VA2IDX(0, DTB_SERIAL)] = page;
//}

//void
//init_vmem()
//{
//	pte_t *root;
//
//	if ((root = (pte_t *)kmalloc()) == 0)
//		panic("couldn't allocate root page table");
//
//	memset(root, '\x00', PAGE_SIZE);	// make every entry invalid
//
//	// TODO: maybe error check???
//	map_page(root, DTB_FLASH, DTB_FLASH, PTE_R | PTE_W);
//	map_page(root, DTB_PLATFORM_BUS, DTB_PLATFORM_BUS, PTE_R | PTE_W);
//	map_page(root, DTB_RTC, DTB_RTC, PTE_R | PTE_W);
//	map_page(root, DTB_SERIAL, DTB_SERIAL, PTE_R | PTE_W);
//	map_page(root, DTB_CLINT, DTB_CLINT, PTE_R | PTE_W);
//
//	// testing
//	// temporarily map direct pages (removed after all init_hart_vmem())
//	uint64 va, pa, size;
//	va = (uint64) text;
//	pa = (uint64) text;
//	size = (uint64) etext - (uint64) text;
//	map_pages(root, va, pa, size, PTE_R | PTE_X);
//	va = (uint64) text - DTB_MEMORY + 0xffffffc000000000;
//	map_pages(root, va, pa, size, PTE_R | PTE_X);
//
//	va = (uint64) rodata;
//	pa = (uint64) rodata;
//	size = (uint64) erodata - (uint64) rodata;
//	map_pages(root, va, pa, size, PTE_R);
//	va = (uint64) rodata - DTB_MEMORY + 0xffffffc000000000;
//	map_pages(root, va, pa, size, PTE_R);
//
//	va = (uint64) data;
//	pa = (uint64) data;
//	size = (uint64) edata - (uint64) data;
//	map_pages(root, va, pa, size, PTE_R | PTE_W);
//	va = (uint64) data - DTB_MEMORY + 0xffffffc000000000;
//	map_pages(root, va, pa, size, PTE_R | PTE_W);
//
//
//	asm volatile("sfence.vma zero, zero");
//	W_SATP(ATP_MODE_Sv39 | (((uint64) root) >> 12));
//
//	uint64 enter_vas = (uint64) &&entered - DTB_MEMORY + 0xffffffc000000000;
//entered:
//	while (1);
//	// end of testing
//
//	kernel_pgtable = root;
//}
