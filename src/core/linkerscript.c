#define __LINKER__
#include "dtb.h"

OUTPUT_ARCH("riscv")
ENTRY(setup)

SECTIONS
{
	. = DTB_MEMORY;

	.text ALIGN(0x1000) : {
		*(.boot)
		*(.text .text.*)
	}

	.data ALIGN(0x1000) : {
		*(.data .data.*)
	}

	.bss ALIGN(0x1000) : {
		*(.bss .bss.*)
	}

	.cpu_stacks ALIGN(0x1000) : {
		*(.cpu_stacks .cpu_stacks.*)
	}
}
