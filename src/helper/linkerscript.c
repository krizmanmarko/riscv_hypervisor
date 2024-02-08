// This is just a normal linker script, but it is edited by C preprocessor
// This enables using include/dtb.h symbols in this script

/*
 * .boot is only used when booting (useless after initial page table is set up)
 *
 * why have seperate boot sections: so i can write explicitly which code is in
 * vas and which in phys as
 *
 * seperate cpu stacks section in the end (so I can map it easier later)
 */

#define __LINKER_SCRIPT__

#include "dtb.h"
#include "memory.h"

OUTPUT_ARCH("riscv")
ENTRY(setup)

SECTIONS
{
	. = DTB_MEMORY + FIRMWARE_SIZE;
	phys_base = .;
	kernel_base = PA2KVA(phys_base);

	/* start boot section */
	/* this is special because VMA == LMA */
	.boot.text ALIGN(PAGE_SIZE) : {
		PROVIDE(boottext = .);
		*(.boot.text.s)		/* to make sure correct function is first */
		*(.boot.text.*)
	}
	PROVIDE(eboottext = .);

	.boot.data ALIGN(PAGE_SIZE) : {
		PROVIDE(bootdata = .);
		*(.boot.data .boot.data.*)
	}
	PROVIDE(ebootdata = .);

	.boot.bsp ALIGN(PAGE_SIZE) : {
		PROVIDE(bsp = .);
		*(.boot.bsp)
	}
	PROVIDE(ebsp = .);

	PROVIDE(eboot = .);

	/* keep physical and virtual memory in sync */
	. = ALIGN(PAGE_SIZE);
	offset = . - phys_base;
	. = kernel_base + offset;

	/* start RX section */
	.text ALIGN(PAGE_SIZE) : AT(phys_base + offset) {
		PROVIDE(text = .);
		*(.text.entry)
		*(.text .text.*)
	}
	PROVIDE(etext = .);
	offset = ALIGN(offset + SIZEOF(.text), PAGE_SIZE);

	/* start RO section in new page */
	.rodata ALIGN(PAGE_SIZE) : AT(phys_base + offset) {
		PROVIDE(rodata = .);
		. = ALIGN(16);
		*(.rodata .rodata.*)
	}
	PROVIDE(erodata = .);
	offset = ALIGN(offset + SIZEOF(.rodata), PAGE_SIZE);

	/* start RW section in new page */
	.data ALIGN(PAGE_SIZE) : AT(phys_base + offset) {
		PROVIDE(data = .);
		. = ALIGN(16);
		*(.data .data.*)
	}
	offset = ALIGN(offset + SIZEOF(.data), PAGE_SIZE);

	.bss ALIGN(PAGE_SIZE) : AT(phys_base + offset) {
		. = ALIGN(16);
		*(.bss .bss.*)
		*(.sbss .sbss.*)
	}
	PROVIDE(edata = .);
	offset = ALIGN(offset + SIZEOF(.bss), PAGE_SIZE);

	.cpu_structs ALIGN(PAGE_SIZE) : AT(phys_base + offset) {
		PROVIDE(cpu_structs = .);
		. = ALIGN(16);
		*(.cpu_structs .cpu_structs.*)
	}
	PROVIDE(ecpu_structs = .);
	offset = ALIGN(offset + SIZEOF(.cpu_structs), PAGE_SIZE);

	.vm_images ALIGN(PAGE_SIZE) : AT(phys_base + offset) {
		*(.vm_image_guest .vm_image_guest.*)
	}

	. = ALIGN(PAGE_SIZE);
	PROVIDE(dynamic = .);	// kernel va

}
