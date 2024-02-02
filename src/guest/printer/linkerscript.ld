OUTPUT_ARCH("riscv")
ENTRY(_start)

SECTIONS
{
	. = 0x80000000;

	.boot : {
		*(.boot .boot.*)
	}

	.text : {
		*(.text .text.*)
	}
}
