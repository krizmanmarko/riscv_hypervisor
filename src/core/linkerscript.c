#define __LINKER__

OUTPUT_ARCH("riscv")
ENTRY(setup)

SECTIONS
{
	. = 0x3000;

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
}
