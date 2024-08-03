#include "dtb.h"
#include "memory.h"
#include "vm_config.h"

// name must start with "vm_" otherwise it will belong to wrong section
EMBED(vm_xv6, "guest/imgs/xv6.bin")
EMBED(vm_keygrab, "guest/imgs/keygrab.bin");
EMBED(vm_printer, "guest/imgs/printer.bin");

struct mmio_dev serial = {
	.base_phys = DTB_SERIAL,
	.base_virt = DTB_SERIAL,
	.size = PAGE_SIZE,
	.perm = PTE_R | PTE_W,
	.phys_irq = 10,
	.virt_irq = 10
};

struct mmio_dev emulated_plic = {
	.base_phys = DTB_PLIC,
	.base_virt = DTB_PLIC,
	.size = DTB_PLIC_SIZE,
	.perm = 0,
	.phys_irq = 0,
	.virt_irq = 0
};

struct mmio_dev virtio_disk = {
	.base_phys = 0x10001000,
	.base_virt = 0x10001000,
	.size = 0x1000,
	.perm = PTE_R | PTE_W,
	.phys_irq = 1,
	.virt_irq = 1
};

struct mmio_dev serial2 = {
	.base_phys = DTB_SERIAL,
	.base_virt = 2*DTB_SERIAL,
	.size = PAGE_SIZE,
	.perm = PTE_R | PTE_W,
	.phys_irq = 10,
	.virt_irq = 11
};

struct mmio_dev emulated_plic2 = {
	.base_phys = DTB_PLIC,
	.base_virt = 2*DTB_PLIC,
	.size = DTB_PLIC_SIZE,
	.perm = 0,
	.phys_irq = 0,
	.virt_irq = 0
};

struct vm_config config[] = {
	{
		.image_base = ADDRESS(vm_xv6),
		.image_end = END_ADDRESS(vm_xv6),
		.cpu_affinity = 0x1,
		.memory_base = 0x80100000,
		.memory_size = 16 << 20,	// 16 megabytes
		.entry = 0x80100000,
		.devices = {
			&serial,
			&emulated_plic,
			&virtio_disk,
			0
		}
	}, {
		.image_base = ADDRESS(vm_printer),
		.image_end = END_ADDRESS(vm_printer),
		.cpu_affinity = 0x0,
		.memory_base = 0x80100000,
		.memory_size = 0x2000,
		.entry = 0x80100000,
		.devices = {
			&serial,
			&emulated_plic,
			0
		}
	}
};

int nr_vms = sizeof(config) / sizeof(struct vm_config);
