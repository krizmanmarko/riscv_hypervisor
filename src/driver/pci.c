#include "dtb.h"
#include "stdio.h"
#include "types.h"

// for now only 1 pci uart is supported
// check out linux's: static int configure_device(struct pci_func *func)

static volatile uint32 *pci_serial = (uint32 *)(DTB_PCI + 0x8000);	// found in gdb
//static volatile uint32 *pci_serial = (uint32 *)(DTB_PCI + 2 * 0x8000);// found in gdb

static void
enable_io_access(volatile uint32 *pci_dev)
{
	uint16 *command = (uint16 *)&pci_dev[1];
	*command = 0x1;		// enable IO
}

// offset from DTB_PCI_IO
static void
set_io_base(volatile uint32 *pci_dev, uint64 offset)
{
	// TODO: check alignment
	pci_dev[4] = offset | 1;
}

void
init_pci()
{
	enable_io_access(pci_serial);
	set_io_base(pci_serial, 0);		// actually requires just 8 bytes, but we skip
	//*((char *)0x3000000) = 'B';	// test (make sure this is mapped in HS)

	// for now only 1 pci uart is supported -> interrupt handling is more difficult with 2
	//enable_io_access(pci_serial_2);
	//set_io_base(pci_serial, PAGE_SIZE);	// PAGE_SIZE so we can just mmap it in VM
	//*((char *)0x3001000) = 'C';	// test
}
