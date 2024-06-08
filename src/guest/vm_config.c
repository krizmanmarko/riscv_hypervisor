#include "vm_config.h"

EMBED(vm_printer, "guest/imgs/printer.bin");
EMBED(vm_printer2, "guest/imgs/printer2.bin");

struct vm_config config[] = {
	{
		.image_base = ADDRESS(vm_printer),
		.image_size = 0x1000,	// TODO: compute this dynamically
		.nr_vcpus = 1,
		.cpu_affinity = 0x1,
		.memory_base = 0x80100000,
		.entry = 0x80100000
	},
	{
		.image_base = ADDRESS(vm_printer2),
		.image_size = 0x1000,
		.nr_vcpus = 1,
		.cpu_affinity = 0x2,
		.memory_base = 0x80100000,
		.entry = 0x80100000
	}
};

int nr_vms = sizeof(config) / sizeof(struct vm_config);
