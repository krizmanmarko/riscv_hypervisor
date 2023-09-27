// This is just a normal linker script, but it is edited by C preprocessor
// This enables using include/dtb.h symbols in this script

#include "dtb.h"
#define PAGE_SIZE 0x1000

OUTPUT_ARCH("riscv")
ENTRY(boot)

MEMORY
{
	RAM : ORIGIN = DTB_MEMORY, LENGTH = DTB_MEMORY_SIZE
}

SECTIONS
{
	. = DTB_MEMORY;

	/* start RX section */
	.boot BLOCK (PAGE_SIZE) : {
		*(.boot .boot.*)
	} > RAM

	.text : {
		*(.text .text.*)
	} > RAM
	. = ALIGN(PAGE_SIZE);
	PROVIDE(etext = .);

	/* start RO section in new page */
	.rodata BLOCK (PAGE_SIZE) : {
		. = ALIGN(16);
		*(.rodata .rodata.*)
	} > RAM
	. = ALIGN(PAGE_SIZE);
	PROVIDE(erodata = .);

	/* start RW section in new page */
	.data BLOCK (PAGE_SIZE) : {
		. = ALIGN(16);
		*(.data .data.*)
	} > RAM

	.bss : {
		. = ALIGN(16);
		*(.bss .bss.*)
	} > RAM

	PROVIDE(end = .);
}
