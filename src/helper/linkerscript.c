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
ENTRY(boot)

SECTIONS
{
	. = DTB_MEMORY;

	/* start boot section */
	/* this is special because VMA == LMA */
	.boot.text ALIGN(PAGE_SIZE) : {
		PROVIDE(boottext = .);
		*(.boot.text.m)		/* to make sure M-mode boot is first */
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
	phys_locctr = .;
	. = . - DTB_MEMORY + VAS_BASE;

	/* start RX section */
	.text ALIGN(PAGE_SIZE) : AT(phys_locctr) {
		PROVIDE(text = .);
		*(.text.entry)
		*(.text .text.*)
	}
	PROVIDE(etext = .);
	phys_locctr = ALIGN(phys_locctr + SIZEOF(.text), PAGE_SIZE);

	/* start RO section in new page */
	.rodata ALIGN(PAGE_SIZE) : AT(phys_locctr) {
		PROVIDE(rodata = .);
		. = ALIGN(16);
		*(.rodata .rodata.*)
	}
	PROVIDE(erodata = .);
	phys_locctr = ALIGN(phys_locctr + SIZEOF(.rodata), PAGE_SIZE);

	/* start RW section in new page */
	.data ALIGN(PAGE_SIZE) : AT(phys_locctr) {
		PROVIDE(data = .);
		. = ALIGN(16);
		PROVIDE(__global_pointer$ = . + 0x800);
		*(.data .data.*)
	}
	phys_locctr = ALIGN(phys_locctr + SIZEOF(.data), PAGE_SIZE);

	.bss ALIGN(PAGE_SIZE) : AT(phys_locctr) {
		. = ALIGN(16);
		*(.bss .bss.*)
		*(.sbss .sbss.*)
	}
	PROVIDE(edata = .);
	phys_locctr = ALIGN(phys_locctr + SIZEOF(.bss), PAGE_SIZE);

	. = ALIGN(PAGE_SIZE);
	PROVIDE(dynamic = .);
}
