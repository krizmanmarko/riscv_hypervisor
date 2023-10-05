// This is just a normal linker script, but it is edited by C preprocessor
// This enables using include/dtb.h symbols in this script

#include "dtb.h"
#include "memory.h"

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
		PROVIDE(text = .);
		*(.boot .boot.*)
	} > RAM

	.text : {
		*(.text .text.*)
	} > RAM
	PROVIDE(etext = .);

	/* start RO section in new page */
	.rodata BLOCK (PAGE_SIZE) : {
		PROVIDE(rodata = .);
		. = ALIGN(16);
		*(.rodata .rodata.*)
	} > RAM
	PROVIDE(erodata = .);

	/* start RW section in new page */
	.data BLOCK (PAGE_SIZE) : {
		PROVIDE(data = .);
		. = ALIGN(16);
		*(.data .data.*)
	} > RAM

	.bss : {
		. = ALIGN(16);
		*(.bss .bss.*)
	} > RAM
	PROVIDE(edata = .);

	PROVIDE(end = .);
}
