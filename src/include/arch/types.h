#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

// only define types that depend on the architecture

#ifndef __ASSEMBLER__

#if __riscv_xlen == 32
typedef uint32_t xlen_t;
#elif __riscv_xlen == 64
typedef uint64_t xlen_t;
typedef uint64_t pte_t;
#endif

#endif // __ASSEMBLER__

#endif // TYPES_H
