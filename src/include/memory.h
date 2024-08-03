#ifndef MEMORY_H
#define MEMORY_H

#include "dtb.h"

// WARNING!
// following section is also used in linker script

// sv39 translation scheme

#define PAGE_SHIFT 12
#define PAGE_SIZE (1 << PAGE_SHIFT)

// DO NOT CHANGE -> stuff breaks...
#define VAS_KERNEL (0xffffffc000000000)
#define VAS_CPU_STRUCT (0xffffffc040000000)
#define VAS_RAM (0xffffffe000000000)
#define MAX_VA (0xffffffffffffffff)

// kpa - kernel physical address (address of linearly mapped RAM)
// pa - physical address (actual physical address)
// kva - kernel virtual address (kernel image address)
#define KPA2PA(kpa) (((kpa) & ~VAS_RAM) + DTB_MEMORY)
#define PA2KPA(pa) (((pa) - DTB_MEMORY) | VAS_RAM)

// kernel_va to pa
#define KVA2PA(kva) ((kva) - VAS_KERNEL + DTB_MEMORY)
#define PA2KVA(pa) ((pa) - DTB_MEMORY + VAS_KERNEL)
#define KVA2KPA(kva) ((kva) - VAS_KERNEL + VAS_RAM)
#define KPA2KVA(kpa) ((kpa) - VAS_RAM + VAS_KERNEL)

// nothing else is needed by linkerscript
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
#define PTE2PA(pte) ((uint64) (((pte) >> 10) << 12))

// vpn[2], vpn[1], vpn[0], offset -> vpn[level]
#define VA2IDX(level, va) ((((va) >> PAGE_SHIFT) >> ((level) * 9)) & 0x1ff)

#endif // __LINKER_SCRIPT__
#endif // MEMORY_H
