#ifndef MEMORY_H
#define MEMORY_H

#define PAGE_SHIFT 12
#define PAGE_SIZE (1 << PAGE_SHIFT)

// DO NOT CHANGE -> stuff breaks...
#define VAS_KERNEL (0xffffffc000000000)
#define VAS_CPU_STRUCT (0xffffffc040000000)
#define VAS_RAM (0xffffffe000000000)


// sv39 translation scheme

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

#endif // MEMORY_H
