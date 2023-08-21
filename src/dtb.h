// This file only exists so I do not have to parse dtb directly

#ifndef DTB_H
#define DTB_H

#define FW_CFG 0x10100000
#define FLASH 0x20000000
#define PLATFORM_BUS 0x4000000

#define MEMORY 0x80000000
#define MEMORY_SIZE 0x20000000

#define NR_CPUS 3

#define RTC 0x101000
#define SERIAL 0x10000000

#define PLIC 0xc000000
#define CLINT 0x2000000

#endif // DTB_H
