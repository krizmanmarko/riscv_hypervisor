// check out https://github.com/riscv/riscv-aclint/blob/main/riscv-aclint.adoc
// riscv-qemu has only one mswi and one mtimer device therefore
//     mswi hartid and cpu hartid are equal
//     mtimer hartid and cpu hartid are equal
// hartid = [0, 4094] <- (4095 devices, 1 slot is reserved)

#ifndef CLINT_H
#define CLINT_H

#include "dtb.h"

#define BASE DTB_CLINT

#define MSWI_BASE BASE
#define MSIP(hartid) (MSWI_BASE + hartid * 4)

#define MTIMER_BASE (BASE + 0x4000)
#define MTIMECMP(hartid) (MTIMER_BASE + hartid * 8)
#define MTIME (MTIMER_BASE + 0x7ff8)

#endif // CLINT_H
