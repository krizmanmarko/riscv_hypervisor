#ifndef VMEM_H
#define VMEM_H

#define PAGE_SIZE 4096

#define PGROUNDUP(a) (((a) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1))
#define PGROUNDDOWN(a) ((a) & ~(PAGE_SIZE - 1))

#define PTE_V (1 << 0)
#define PTE_R (1 << 1)
#define PTE_W (1 << 2)
#define PTE_X (1 << 3)
#define PTE_U (1 << 4)
#define PTE_G (1 << 5)
#define PTE_A (1 << 6)
#define PTE_D (1 << 7)

// get ppn to correct offset
#define PA2PTE(pa) ((pa >> 12) << 10)
#define PTE2PA(pte) ((pte >> 10) << 12)

#endif // VMEM_H
