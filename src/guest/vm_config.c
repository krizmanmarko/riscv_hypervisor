#include "vm_config.h"

// name must start with "vm_" otherwise it will belong to wrong section
EMBED(vm_printer, "guest/imgs/printer.bin");
EMBED(vm_printer2, "guest/imgs/printer.bin");
//EMBED(vm_keygrab, "guest/imgs/keygrab.bin");

struct vm_config config[] = {
	{
		.image_base = ADDRESS(vm_printer),
		.image_size = 0x1000,	// TODO: compute this dynamically
		.cpu_affinity = 0x3,
		.memory_base = 0x80100000,
		.memory_size = 0x1000,
		.entry = 0x80100000,
	},
	{
		.image_base = ADDRESS(vm_printer2),
		.image_size = 0x1000,
		.cpu_affinity = 0xc,
		.memory_base = 0x80100000,
		.memory_size = 0x1000,
		.entry = 0x80100000
	}
};

int nr_vms = sizeof(config) / sizeof(struct vm_config);
