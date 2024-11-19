#ifndef VM_CONFIG_H
#define VM_CONFIG_H

#include "lock.h"
#include "memory.h"
#include "types.h"
#include "vcpu.h"

#define MAX_DEVS 10
#define ADDRESS(vm_image) ((uint64) KVA2PA(vm_image))
#define END_ADDRESS(vm_image) ((uint64) KVA2PA(vm_image##_end))

// https://developers.redhat.com/blog/2019/07/05/how-to-store-large-amounts-of-data-in-a-program#the_incbin_directive
// Note: cannot have do {} while (0) because it is used in top namespace.
#define EMBED(img_name, img_path) \
	__asm__( \
		".pushsection ." #img_name ", \"a\", @progbits;" \
		".globl " #img_name ";" \
		#img_name ":" \
		".incbin " #img_path ";" \
		".globl " #img_name "_end;" \
		#img_name "_end:" \
		".popsection;" \
	); \
	extern char img_name[]; \
	extern char img_name##_end[];

struct mmio_dev {
	uint64 phys_base;
	uint64 virt_base;
	uint64 size;
	uint64 perm;	// emulated devices must have 0 permissions
	int phys_irq;	// actual interrupt number (0 means no interrupt)
	int virt_irq;	// virtualised interrupt number (1:1 mapping)
};

struct vm_config {
	// config
	uint64 cpu_affinity;
	uint64 image_base;	// where binary begins in KPA
	uint64 image_end;	// where binary ends in KPA
	uint64 memory_base;	// location of RAM (gpa)
	uint64 memory_size;	// allocated RAM for this VM
	uint64 entry;
	struct mmio_dev *devices[MAX_DEVS];	// NULL terminated array

	// per vm memory allocations
	struct barrier bar;
	pte_t vm_pgtable[512 * 4] __attribute__((aligned(4 * PAGE_SIZE)));
};

extern int nr_vms;
extern struct vm_config config[];

#endif // VM_CONFIG_H
