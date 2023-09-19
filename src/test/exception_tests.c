#include "dtb.h"
#include "riscv_hypervisor.h"

extern uint64 *access_fault_test_addr;

void
exc_instruction_address_misaligned_test()
{
	// privileged isa mcause section:
	// Instruction address misaligned exceptions are raised by
	// control-flow instructions with misaligned targets, rather than by
	// the act of fetching an instruction. Therefore, these exceptions
	// have lower priority than other instruction address exceptions.
	asm volatile("addi ra, ra, 1");
	// I tried "j misaligned_addr" but it does not link
	//asm volatile("j misaligned");
	//asm volatile(".balign 4");
	//asm volatile(".byte 1");
	//asm volatile("misaligned: j misaligned");
}

void
exc_instruction_access_fault_test()
{
	asm volatile("li ra, 0x9ffffff8");
	//asm volatile("mv ra, %0" : : (access_fault_test_addr));
}

void
exc_load_access_fault_test()
{
	asm volatile("li t0, 0x9ffffff8");
	asm volatile("ld t0, 0(t0)");
}

void
exc_store_or_amo_access_fault_test()
{
	asm volatile("li t0, 0x9ffffff8");
	asm volatile("sd zero, 0(t0)");
}
