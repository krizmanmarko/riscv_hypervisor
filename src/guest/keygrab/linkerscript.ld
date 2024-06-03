OUTPUT_ARCH("riscv")
ENTRY(_start)

SECTIONS
{
	. = 0x80100000;

	.boot : {
		*(.boot .boot.*)
	}

	.text : {
		*(.text .text.*)
	}

	. = ALIGN(4K);
	.data : {
		*(.data .data.*)
	}
}
