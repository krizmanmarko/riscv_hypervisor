OUTPUT_ARCH("riscv")
ENTRY(boot)

SECTIONS
{
	. = 0x80000000;

	.text : {
		*(.text .text.*)
		. = ALIGN(0x1000);
		PROVIDE(etext = .);
	}

	.rodata : {
		. = ALIGN(16);
		*(.rodata .rodata.*)
	}

	.data : {
		. = ALIGN(16);
		*(.data .data.*)
	}

	.bss : {
		. = ALIGN(16);
		*(.bss .bss.*)
	}

	PROVIDE(end = .);

	# DTB_MEMORY + DTB_MEMORY_SIZE - 8
	. = 0xa0000000 - 8;
	.nop : {
		*(.nop .nop.*)
	}
}
