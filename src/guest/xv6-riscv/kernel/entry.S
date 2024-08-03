        # qemu -kernel loads the kernel at 0x80000000
        # and causes each hart (i.e. CPU) to jump there.
        # kernel.ld causes the following code to
        # be placed at 0x80000000.
.section .text
.global _entry
# a0 holds hartid
# a1 holds dtb address (if not aligned to 0x100, use paravirt drivers)
_entry:
	mv tp, a0
	la a0, use_paravirtualized_driver
	sb a1, 0(a0)
        # set up a stack for C.
        # stack0 is declared in start.c,
        # with a 4096-byte stack per CPU.
        # sp = stack0 + (hartid * 4096)
        la sp, stack0
        li a0, 1024*4
        mv a1, tp
        addi a1, a1, 1
        mul a0, a0, a1
        add sp, sp, a0
        # jump to main() in main.c
        call main
spin:
        j spin
