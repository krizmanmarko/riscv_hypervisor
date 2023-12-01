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
static int
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
	for (uint64 i = 0; i < PGROUNDUP(size); i += PAGE_SIZE)
		if (map_page(pgtable, va + i, pa + i, pte_flags) < 0)
			return -1;
	return 0;
}

// creates page table supervisor will actually use
void
init_vmem()
{
	pte_t *root;
	int rv, size;
	uint64 va, pa;

	if ((root = (pte_t *)kmalloc()) == 0)
		panic("couldn't allocate root page table");

	memset(root, '\x00', PAGE_SIZE);	// make every entry invalid

	rv = map_page(root, DTB_FLASH, DTB_FLASH, PTE_R | PTE_W);
	rv += map_page(root, DTB_PLATFORM_BUS, DTB_PLATFORM_BUS, PTE_R | PTE_W);
	rv += map_page(root, DTB_RTC, DTB_RTC, PTE_R | PTE_W);
	rv += map_page(root, DTB_SERIAL, DTB_SERIAL, PTE_R | PTE_W);
	rv += map_page(root, DTB_CLINT, DTB_CLINT, PTE_R | PTE_W);
	if (rv < 0)
		panic("failed to map %s", "MMIO");

	// map kernel image
	va = (uint64) text;
	pa = VA2PA((uint64) text);
	size = (unsigned int) (etext - text);
	if (map_pages(root, va, pa, size, PTE_R | PTE_X))
		panic("failed to map %s", ".text");

	va = (uint64) rodata;
	pa = VA2PA((uint64) rodata);
	size = (unsigned int) (erodata - rodata);
	if (map_pages(root, va, pa, size, PTE_R))
		panic("failed to map %s", ".rodata");

	va = (uint64) data;
	pa = VA2PA((uint64) data);
	size = (unsigned int) (edata - data);
	if (map_pages(root, va, pa, size, PTE_R | PTE_W))
		panic("failed to map %s", "data");

	if (map_pages(root, DTB_MEMORY, DTB_MEMORY, DTB_MEMORY_SIZE, PTE_R | PTE_W | PTE_X))
		panic("failed to map %s", "RAM");

	kernel_pgtable = root;
}


// TODO: this is just for testing


pte_t vm_pgtable[512 * 4] __attribute__((aligned(4 * PAGE_SIZE)));

void
vm_run()
{
	memset(vm_pgtable, '\x00', PAGE_SIZE * 4);	// make every entry invalid
	map_page(vm_pgtable, DTB_SERIAL, DTB_SERIAL, PTE_U | PTE_R | PTE_W);
	map_page(vm_pgtable, DTB_MEMORY, DTB_MEMORY + FIRMWARE_SIZE + 0x11000, PTE_U | PTE_R | PTE_X);
	W_HGATP(ATP_MODE_Sv39 | ((VA2PA((uint64) vm_pgtable)) >> 12));
	asm volatile("hfence.gvma");	// TODO: needed?

	uint64 hstatus = 0;
	hstatus |= 2ULL << 32;		// vsxlen: 2 == 64-bit
	hstatus |= HSTATUS_SPV;		// go into virtualized env
	W_HSTATUS(hstatus);

	uint64 sstatus = 0;
	sstatus |= SSTATUS_SPP;
	//sstatus |= 0xf << 13;	// XS dirty, FS dirty
	W_SSTATUS(sstatus);
	W_SEPC(0x80000000ULL);

	W_HTIMEDELTA(0ULL);
	W_VSSTATUS(STATUS_SD | (0xf << 13));
	W_HIE(0ULL);
	W_VSTVEC(0ULL);
	W_VSSCRATCH(0ULL);
	W_VSEPC(0ULL);
	W_VSCAUSE(0ULL);
	W_VSTVAL(0ULL);
	W_HVIP(0ULL);
	W_VSATP(0ULL);

	asm volatile("sret");
}
