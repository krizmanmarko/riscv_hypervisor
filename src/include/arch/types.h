#ifndef TYPES_H
#define TYPES_H

#ifndef __ASSEMBLER__

#include <stdint.h>

// only define types that depend on the architecture
#if __riscv_xlen == 32
typedef xlen_t uint32_t
#elif __riscv_xlen == 64
typedef xlen_t uint64_t
typedef uint64_t pte_t;
#endif

#endif // __ASSEMBLER__

#endif // TYPES_H
