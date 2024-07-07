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
	.base_virt = DTB_SERIAL,
	.size = PAGE_SIZE,
	.perm = PTE_R | PTE_W,
	.irq_phys = 10,
	.irq_virt = 10
};

struct mmio_dev emulated_plic = {
	.base_phys = DTB_PLIC,
	.base_virt = DTB_PLIC,
	.size = DTB_PLIC_SIZE,
	.perm = 0,
	.irq_phys = 0,
	.irq_virt = 0
};

struct vm_config config[] = {
	{
		.image_base = ADDRESS(vm_keygrab),
		.image_size = 0x1000,	// TODO: compute this dynamically
		.cpu_affinity = 0x1,
		.memory_base = 0x80100000,
		.memory_size = 0x1000,
		.entry = 0x80100000,
		.devices = {
			&serial,
			&emulated_plic,
			0
		}
	}
};

int nr_vms = sizeof(config) / sizeof(struct vm_config);
