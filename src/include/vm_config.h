#ifndef VM_CONFIG_H
#define VM_CONFIG_H

#include "types.h"

// https://developers.redhat.com/blog/2019/07/05/how-to-store-large-amounts-of-data-in-a-program#the_incbin_directive
// Note: cannot have do {} while (0) because it is used in top namespace.
#define EMBED(img_name, img_path) \
	extern char vm_##img_name##_start[]; \
	extern char vm_##img_name##_end[]; \
	__asm__( \
		".pushsection .vm_" #img_name ", \"a\", @progbits;" \
		".globl vm_" #img_name "_start;" \
		"vm_" #img_name "_start:" \
		".incbin " #img_path ";" \
		".globl vm_" #img_name "_end;" \
		"vm_" #img_name "_end:" \
		".popsection;" \
	)

struct vm_config {
	int nr_vcpus;
	int cpu_affinity;
	uint64 memory_base;
	uint64 memory_size;
	uint64 entry;
	//devices

	// this should not be part of vm config but part of kernel code
	// somehow know where this image is in phyisical RAM (for hgatp)
};

#endif // VM_CONFIG_H
