#include "vm_config.h"

EMBED(vm_printer, "guest/imgs/printer.bin");
EMBED(vm_printer_d, "guest/imgs/printer_d.bin");

struct config config = {
	.nr_vms = NR_VMS,	// this is a macro because it is easier
	.vm = {
		{
			.image_base = ADDRESS(vm_printer),
			.nr_vcpus = 1,
			.cpu_affinity = 0x1,
			.memory_base = 0x80100000,
			.memory_size = 0x2000,
			.entry = 0x80100000
		},
		{
			.image_base = ADDRESS(vm_printer_d),
			.nr_vcpus = 1,
			.cpu_affinity = 0x2,
			.memory_base = 0x80100000,
			.memory_size = 0x2000,
			.entry = 0x80100000
		}
	}
};
