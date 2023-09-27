// This file only exists so I do not have to parse dtb directly

// WARNING!
// constants must also be compliant with linker script
// 1UL -> fails in linker script
// 1   -> works in linker script

#ifndef DTB_H
#define DTB_H

#define DTB_FW_CFG 0x10100000
#define DTB_FLASH 0x20000000
#define DTB_PLATFORM_BUS 0x4000000

#define DTB_MEMORY 0x80000000
#define DTB_MEMORY_SIZE 0x20000000

#define DTB_NR_CPUS 3

#define DTB_RTC 0x101000
#define DTB_SERIAL 0x10000000

#define DTB_PLIC 0xc000000
#define DTB_CLINT 0x2000000

#endif // DTB_H
