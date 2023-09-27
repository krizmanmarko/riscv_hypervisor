#include "dtb.h"

.section ".data", "aw"
	.balign 8
	.byte 0x5a
misaligned:
	.dword 0x5a5a5a5a5a5a5a5a

.section ".text", "ax"
.globl exc_instruction_address_misaligned_test
.globl exc_instruction_access_fault_test
.globl exc_illegal_instruction_test
.globl exc_breakpoint_test
.globl exc_load_address_misaligned_test
.globl exc_load_access_fault_test
.globl exc_store_or_amo_address_misaligned_test
.globl exc_store_or_amo_access_fault_test

# 0
exc_instruction_address_misaligned_test:
	addi ra, ra, 1
	ret

# 1
exc_instruction_access_fault_test:
	li ra, DTB_MEMORY + DTB_MEMORY_SIZE - 4

# 2
exc_illegal_instruction_test:
	unimp

# 3
exc_breakpoint_test:
	ebreak

# 4
exc_load_address_misaligned_test:
	la a0, misaligned
	ld a0, 0(a0)
	ret

# 5
exc_load_access_fault_test:
	li a0, DTB_MEMORY + DTB_MEMORY_SIZE - 4
	lw a0, 0(a0)

# 6
exc_store_or_amo_address_misaligned_test:
	la a0, misaligned
	# qemu-virt allows misaligned stores
	sd zero, 0(a0)
	# no ZAM extension -> fail
	amoxor.d a0, a0, 0(a0)

# 7
exc_store_or_amo_access_fault_test:
	li a0, DTB_MEMORY + DTB_MEMORY_SIZE - 4
	sw zero, 0(a0)
