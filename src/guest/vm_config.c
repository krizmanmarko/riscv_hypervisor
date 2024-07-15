#include "dtb.h"
#include "memory.h"
#include "vm_config.h"

// name must start with "vm_" otherwise it will belong to wrong section
EMBED(vm_test_plic_emulation, "guest/imgs/test_plic_emulation.bin")
EMBED(vm_printer, "guest/imgs/printer.bin");
EMBED(vm_printer2, "guest/imgs/printer.bin");
EMBED(vm_keygrab, "guest/imgs/keygrab.bin");

struct mmio_dev serial = {
	.base_phys = DTB_SERIAL,
	.base_virt = 2 * DTB_SERIAL,
	.size = PAGE_SIZE,
	.perm = PTE_R | PTE_W,
	.phys_irq = 10,
	.virt_irq = 11
};

struct mmio_dev emulated_plic = {
	.base_phys = DTB_PLIC,
	.base_virt = 2 * DTB_PLIC,
	.size = DTB_PLIC_SIZE,
	.perm = 0,
	.phys_irq = 0,
	.virt_irq = 0
};

struct mmio_dev serial_2 = {
	.base_phys = DTB_SERIAL,
	.base_virt = DTB_SERIAL,
	.size = PAGE_SIZE,
	.perm = PTE_R | PTE_W,
	.phys_irq = 0,
	.virt_irq = 0
};

struct vm_config config[] = {
	{
		.image_base = ADDRESS(vm_keygrab),
		.image_size = 0x1000,	// TODO: compute this dynamically
		.cpu_affinity = 0x2,
		.memory_base = 0x80100000,
		.memory_size = 0x1000,
		.entry = 0x80100000,
		.devices = {
			&serial,
			&emulated_plic,
			0
		}
	},
	{
		.image_base = ADDRESS(vm_printer),
		.image_size = 0x1000,
		.cpu_affinity = 0x1,
		.memory_base = 0x80100000,
		.memory_size = 0x2000,
		.entry = 0x80100000,
		.devices = {
			&serial_2,
			0
		}
	}
};

int nr_vms = sizeof(config) / sizeof(struct vm_config);
