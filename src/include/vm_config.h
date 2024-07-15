#ifndef VM_CONFIG_H
#define VM_CONFIG_H

#include "lock.h"
#include "memory.h"
#include "types.h"
#include "vcpu.h"

#define MAX_DEVS 10
#define ADDRESS(vm_image_start) ((uint64) KVA2PA(vm_image_start))

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
	uint64 base_phys;
	uint64 base_virt;
	uint64 size;
	uint64 perm;	// emulated devices must have 0 permissions
	int phys_irq;	// actual interrupt number (0 means no interrupt)
	int virt_irq;	// virtualised interrupt number (1:1 mapping)
};

struct vm_config {
	// config
	uint64 cpu_affinity;
	uint64 image_base;	// where binary in physical memory
	uint64 image_size;	// size of binary
	uint64 memory_base;	// location of RAM
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
