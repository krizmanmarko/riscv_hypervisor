// This is just a normal linker script, but it is edited by C preprocessor
// This enables using include/dtb.h symbols in this script

/*
 * .boot is only used when booting (useless after initial page table is set up)
 *
 * why have seperate boot sections: so i can write explicitly which code is
 * executed in virtual address space and which in physical address space
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
		*(.boot.text.s)		/* make sure correct function is first */
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
		*(.rodata .rodata.*)
		*(.srodata .srodata.*)
	}
	PROVIDE(erodata = .);
	offset = ALIGN(offset + SIZEOF(.rodata), PAGE_SIZE);

	/* start RW section in new page */
	.data ALIGN(PAGE_SIZE) : AT(phys_base + offset) {
		PROVIDE(data = .);
		*(.data .data.*)
		*(.sdata .sdata.*)
		*(.bss .bss.*)
		*(.sbss .sbss.*)
	}
	PROVIDE(edata = .);
	offset = ALIGN(offset + SIZEOF(.data), PAGE_SIZE);

	.cpu_structs ALIGN(PAGE_SIZE) : AT(phys_base + offset) {
		PROVIDE(cpu_structs = .);
		*(.cpu_structs .cpu_structs.*)
	}
	PROVIDE(ecpu_structs = .);
	offset = ALIGN(offset + SIZEOF(.cpu_structs), PAGE_SIZE);

	.vm_images ALIGN(PAGE_SIZE) : AT(phys_base + offset) SUBALIGN(PAGE_SIZE) {
		KEEP(*(.vm_*))
	}

	. = ALIGN(PAGE_SIZE);
	PROVIDE(dynamic = .);	// kernel va
}
