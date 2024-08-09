#include "dtb.h"
#include "stdio.h"
#include "types.h"

//Physical_Address = ECAM_base + ((Bus) << 20 | Device << 15 | Function << 12)

// 1. device discovery
// 2. implement r w to pci serial, possibly only use uart0 for HS
// x. then we also need emulation (xv6 uses UART0, but in reality it will use PCI uart)


// check out linux's: static int configure_device(struct pci_func *func)

void
print_ecam(volatile uint32 *pci)
{
	uint64 *p = (uint64 *)pci;
	for (int i = 0; i < 32; i++)
		printf("%p: %p\n", &p[i], p[i]);
}

volatile uint32 *ecam = (uint32 *)DTB_PCI;
volatile uint32 *pci_serial = (uint32 *)(DTB_PCI + 0x8000);
volatile uint32 *pci_serial2 = (uint32 *)(DTB_PCI + 2 * 0x8000);

void
examine_pci_device(volatile uint32 *pci_dev)
{
	uint32 tmp;
	print_ecam(pci_dev);
	for (int i = 0; i < 6; i++) {
		tmp = pci_dev[4 + i];
		pci_dev[4 + i] = 0xfffffffe;	// bit 1 is 0 if IO is used
		if (pci_dev[4 + i]) {
			printf("bar%d active: %p\n", i, (void *)(uint64)pci_serial[4 + i]);
		}
		pci_dev[4 + i] = tmp;
	}
}

void
enable_io_access(volatile uint32 *pci_dev)
{
	uint16 *command = (uint16 *)&pci_dev[1];
	*command = 0x1;		// enable IO
}

void
enable_irq(volatile uint32 *pci_dev)
{
	uint16 *interrupt_line = (uint16 *)&pci_dev[16];
	*interrupt_line = 0x20;
}

void
init_pci()
{
	enable_io_access(pci_serial);
	enable_irq(pci_serial);
	*((char *)0x3000000) = 'B';

	//enable_io_access(pci_serial2);
//	pci_serial2[4] = 0x1000;	// BAR0 sets the offset from 0x3000000
	//*((char *)0x3001000) = 'C';
	//panic("hello from PCI %p", pci_serial);
	// TODO: this is a big mess
}
