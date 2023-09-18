#ifndef CLINT_H
#define CLINT_H

#include "dtb.h"

#define BASE DTB_CLINT
#define MTIMECMP(hartid) (BASE + 0x4000 + 8 * hartid)
#define MTIME (BASE + 0xBFF8)

#endif // CLINT_H
