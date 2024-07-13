

//Physical_Address = ECAM_base + ((Bus) << 20 | Device << 15 | Function << 12)

// 1. device discovery
// 2. implement r w to pci serial, possibly only use uart0 for HS
// x. then we also need emulation (xv6 uses UART0, but in reality it will use PCI uart)
