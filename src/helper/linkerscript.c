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

MEMORY
{
	RAM (rwx): ORIGIN = DTB_MEMORY, LENGTH = DTB_MEMORY_SIZE
	VAS (rwx): ORIGIN = VAS_BASE, LENGTH = VAS_SIZE
}


SECTIONS
{
	. = DTB_MEMORY;

	/* start boot section */
	/* this is special because VMA == LMA */
	.boot.text BLOCK (PAGE_SIZE) : {
		PROVIDE(boottext = .);
		*(.boot.text.m)		/* to make sure M-mode boot is first */
		*(.boot.text.*)
	}> RAM AT> RAM
	PROVIDE(eboottext = .);

	.boot.data BLOCK (PAGE_SIZE) : {
		PROVIDE(bootdata = .);
		*(.boot.data .boot.data.*)
	}> RAM AT> RAM
	PROVIDE(ebootdata = .);

	.boot.bsp BLOCK (PAGE_SIZE) : {
		PROVIDE(bsp = .);
		*(.boot.bsp)
	}> RAM AT> RAM
	PROVIDE(ebsp = .);

	PROVIDE(eboot = .);
	/*
	 * pa = va - VAS_BASE + eboot
	 * va = pa - eboot + VAS_BASE
	 */


	/* From here on out every symbol provided is virtual address */

	/* simplify PA2VA and VA2PA */
	. = . - DTB_MEMORY + VAS_BASE;

	/* start RX section */
	.text BLOCK (PAGE_SIZE) : {
		PROVIDE(text = .);
		*(.text.entry)
		*(.text .text.*)
	}> VAS AT> RAM
	PROVIDE(etext = .);

	/* start RO section in new page */
	.rodata BLOCK (PAGE_SIZE) : {
		PROVIDE(rodata = .);
		. = ALIGN(16);
		*(.rodata .rodata.*)
	}> VAS AT> RAM
	PROVIDE(erodata = .);

	/* start RW section in new page */
	.data BLOCK (PAGE_SIZE) : {
		PROVIDE(data = .);
		. = ALIGN(16);
		PROVIDE(__global_pointer$ = . + 0x800);
		*(.data .data.*)
	}> VAS AT> RAM

	.bss BLOCK (PAGE_SIZE): {
		. = ALIGN(16);
		*(.bss .bss.*)
	}> VAS AT> RAM
	PROVIDE(edata = .);

	PROVIDE(dynamic = .);
}
