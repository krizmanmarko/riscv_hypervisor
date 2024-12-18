#include <cpu.h>

.section ".boot.entry", "ax"

.globl setup
setup:
	# install exception handler as early as possible
	la t0, early_handler
	csrw stvec, t0

	# store hartid in sscratch (for early boot)
	csrw sscratch, a0

	call early_mem_prot_init
	call load_init_for_c
	jr a0	# init_for_c


# loading symbols from VAS is problematic on 64-bit arch (load too far)

#if __riscv_xlen == 32
.section ".boot.text", "ax"

load_init_for_c:
	la a0, init_for_c
	ret

#elif __riscv_xlen == 64
.section ".boot.rodata", "a"
init_for_c_sym: .dword init_for_c

.section ".boot.text", "ax"

load_init_for_c:
	la a0, init_for_c_sym
	ld a0, 0(a0)
	ret

#endif // __riscv_xlen


.section ".boot.text", "ax"

.balign 4
early_handler:
	j .

.section ".text", "ax"

.globl init_for_c
init_for_c:
	# init stack - last used, grows towards smaller, 16-byte aligned
	la sp, cpu_stacks
	li a0, CPU_STACK_SIZE
	csrr a1, sscratch
	add a1, a1, 1
	mul a0, a0, a1
	add sp, sp, a0
	j early_boot



/*
##### EARLY BOOT PROCESS (master hart only) #####

call early_mem_prot_init
	MMU:	bootstrap pgtables + enter VAS
	MPU:	nop
j init_for_C
j main
call init_uart
call mem_prot_init
	MMU:	kmem
		vmem
		final pgtables + relocate stack
		build pgtables for other harts + wake them up
	MPU:	PMP
*/
