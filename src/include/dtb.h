// This file only exists so I do not have to parse dtb directly

#ifndef DTB_H
#define DTB_H

// WARNING!
// constants must also be compliant with linker script
// 1UL -> fails in linker script
// 1   -> works in linker script

#define DTB_FW_CFG 0x10100000
#define DTB_FLASH 0x20000000
#define DTB_PLATFORM_BUS 0x4000000

#define FIRMWARE_SIZE 0x00100000
#define DTB_MEMORY 0x80000000
#define DTB_MEMORY_SIZE 0x10000000
#define DTB_MEMORY_END (DTB_MEMORY + DTB_MEMORY_SIZE)

#define DTB_NR_CPUS 2

#define DTB_RTC 0x101000
#define DTB_SERIAL 0x10000000
#define DTB_VIRTIO0 0x10001000
#define DTB_VIRTIO_SIZE 0x1000
#define DTB_PCI 0x30000000
#define DTB_PCI_SIZE 0x10000000
#define DTB_PCI_IO 0x3000000
#define DTB_PCI_IO_SIZE 0x10000

#define DTB_PLIC 0xc000000
#define DTB_PLIC_SIZE 0x600000

#define DTB_CLINT 0x2000000
#define DTB_CLINT_SIZE 0x10000

#endif // DTB_H
