#define __LINKER__

#include "dtb.h"
#include "mem.h"

// mpu systems do not define this
#ifndef VAS_KERNEL
#define VAS_KERNEL DTB_MEMORY
#endif

OUTPUT_ARCH("riscv")
ENTRY(setup)

SECTIONS
{
	phys_base = DTB_MEMORY + FIRMWARE_SIZE;
	kernel_base = VAS_KERNEL + FIRMWARE_SIZE;

	. = phys_base;

	/* boot sections are only used during early boot */
	.boot ALIGN(PAGE_SIZE) : {
		*(.boot.entry .boot.entry.*)
		*(.boot.text .boot.text.*)
		*(.boot.rodata .boot.rodata.*)
	}

	/* alignment constraint for page tables */
	.boot.bsp ALIGN(PAGE_SIZE) : {
		*(.boot.bsp .boot.bsp.*)
	}

	/* keep physical and virtual memory in sync */

	. = ALIGN(PAGE_SIZE);
	offset = . - phys_base;
	. = kernel_base + offset;

	/* start RX section */
	.text ALIGN(PAGE_SIZE) : AT(phys_base + offset) {
		//PROVIDE(text = .);
		*(.text.entry)
		*(.text .text.*)
	}
	//PROVIDE(etext = .);
	offset = ALIGN(offset + SIZEOF(.text), PAGE_SIZE);

	/* start RO section in new page */
	.rodata ALIGN(PAGE_SIZE) : AT(phys_base + offset) {
		//PROVIDE(rodata = .);
		*(.rodata .rodata.*)
		*(.srodata .srodata.*)
	}
	//PROVIDE(erodata = .);
	offset = ALIGN(offset + SIZEOF(.rodata), PAGE_SIZE);

	/* start RW section in new page */
	.data ALIGN(PAGE_SIZE) : AT(phys_base + offset) {
		//PROVIDE(data = .);
		*(.data .data.*)
		*(.sdata .sdata.*)
		*(.bss .bss.*)
		*(.sbss .sbss.*)
	}
	//PROVIDE(edata = .);
	offset = ALIGN(offset + SIZEOF(.data), PAGE_SIZE);

	.cpu_structs ALIGN(PAGE_SIZE) : AT(phys_base + offset) {
		//PROVIDE(cpu_structs = .);
		*(.cpu_structs .cpu_structs.*)
	}
	//PROVIDE(ecpu_structs = .);
	offset = ALIGN(offset + SIZEOF(.cpu_structs), PAGE_SIZE);

	.vm_images ALIGN(PAGE_SIZE) : AT(phys_base + offset) SUBALIGN(PAGE_SIZE) {
		KEEP(*(.vm_*))
	}

	. = ALIGN(PAGE_SIZE);
	//PROVIDE(dynamic = .);	// kernel va
}
