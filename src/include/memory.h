#ifndef MEMORY_H
#define MEMORY_H

#include "dtb.h"

// WARNING!
// following section is also used in linker script

// sv39 translation scheme

#define PAGE_SHIFT 12
#define PAGE_SIZE (1 << PAGE_SHIFT)

#define VAS_BASE (0xffffffc000000000)
#define VAS_CPU_STACK (0xffffffc040000000)
#define VAS_SIZE (MAX_VA - VAS_BASE)
#define MAX_VA (0xffffffffffffffff)

#define VA2PA(va) ((va) - VAS_BASE + DTB_MEMORY)
#define PA2VA(pa) ((pa) - DTB_MEMORY + VAS_BASE)

// everything else is not needed in linkerscript
#ifndef __LINKER_SCRIPT__

#define PGROUNDUP(a) (((a) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1))
#define PGROUNDDOWN(a) ((a) & ~(PAGE_SIZE - 1))

#define PTE_SIZE 8
#define LEVELS 3

#define PTE_V (1UL << 0)
#define PTE_R (1UL << 1)
#define PTE_W (1UL << 2)
#define PTE_X (1UL << 3)
#define PTE_U (1UL << 4)
#define PTE_G (1UL << 5)
#define PTE_A (1UL << 6)
#define PTE_D (1UL << 7)

// get ppn to correct offset
#define PA2PTE(pa) ((pte_t) (((pa) >> 12) << 10))
#define PTE2PA(pte) ((pte_t *) (((pte) >> 10) << 12))

// vpn[2], vpn[1], vpn[0], offset -> vpn[level]
#define VA2IDX(level, va) ((((va) >> PAGE_SHIFT) >> ((level) * 9)) & 0x1ff)


#endif // __LINKER_SCRIPT__
#endif // MEMORY_H
