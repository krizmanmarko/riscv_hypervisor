#include <dtb.h>
#include <types.h>

// plic is a constant pointer to int (and not pointer to constant int)
static volatile uint32 *const plic = (uint32 *)DTB_PLIC;

uint32
plic_get_priority(int interrupt_source)
{
	if (interrupt_source < 1 || 1023 < interrupt_source)
		return 0;
	return plic[interrupt_source];
}

// 0 - disabled
// higher the number - higher the priority
void
plic_set_priority(int interrupt_source, uint32 priority)
{
	if (interrupt_source < 1 || 1023 < interrupt_source)	// 0 not allowed
		return;
	plic[interrupt_source] = priority;
}

uint32
plic_get_pending(int interrupt_source)
{
	int offset;
	uint32 mask;

	if (interrupt_source < 1 || 1023 < interrupt_source)	// 0 not allowed
		return 0;

	offset = 0x1000 / sizeof(uint32);	// base of pending regs
	offset += interrupt_source / (sizeof(uint32) * 8);
	mask = 1 << interrupt_source % (sizeof(uint32) * 8);
	return plic[offset] & mask;
}

uint32
plic_get_enabled(int context, int interrupt_source)
{
	int offset;
	int source_offset;
	uint32 source_mask;
	int context_offset;

	if (interrupt_source < 1 || 1023 < interrupt_source)	// 0 not allowed
		return 0;
	source_offset = interrupt_source / (sizeof(uint32) * 8);
	source_mask = 1 << interrupt_source % (sizeof(uint32) * 8);

	if (context < 0 || 15871 < context)
		return 0;
	context_offset = 1024 / (sizeof(uint32) * 8);

	offset = 0x2000 / sizeof(uint32);
	offset += context_offset * context;
	offset += source_offset;
	return plic[offset] & source_mask;
}

void
plic_set_enabled(int context, int interrupt_source, int enable)
{
	int offset;
	int source_offset;
	uint32 source_mask;
	int context_offset;

	if (interrupt_source < 1 || 1023 < interrupt_source)	// 0 not allowed
		return;
	source_offset = interrupt_source / (sizeof(uint32) * 8);
	source_mask = 1 << interrupt_source % (sizeof(uint32) * 8);

	if (context < 0 || 15871 < context)
		return;
	context_offset = 1024 / (sizeof(uint32) * 8);

	offset = 0x2000 / sizeof(uint32);
	offset += context_offset * context;
	offset += source_offset;
	if (enable == 0)
		__asm__ volatile ("amoand.w %0, %[rs1], (%[rs2]);" :: [rs1] "r" (~source_mask), [rs2] "r" (&plic[offset]));
	else
		__asm__ volatile ("amoor.w %0, %[rs1], (%[rs2]);" :: [rs1] "r" (source_mask), [rs2] "r" (&plic[offset]));
}

uint32
plic_get_threshold(int context)
{
	int offset;
	int context_offset;

	if (context < 0 || 15871 < context)
		return 0;
	context_offset = 0x1000 / sizeof(uint32);

	offset = 0x200000 / sizeof(uint32);
	offset += context_offset * context;
	return plic[offset];
}

void
plic_set_threshold(int context, uint32 threshold)
{
	int offset;
	int context_offset;

	if (context < 0 || 15871 < context)
		return;
	context_offset = 0x1000 / sizeof(uint32);

	offset = 0x200000 / sizeof(uint32);
	offset += context_offset * context;
	plic[offset] = threshold;
}

// returns 0 if no interrupt is pending or context error
uint32
plic_claim(int context)
{
	int offset;
	int context_offset;

	if (context < 0 || 15871 < context)
		return 0;
	context_offset = 0x1000 / sizeof(uint32);

	offset = 0x200004 / sizeof(uint32);
	offset += context_offset * context;
	return plic[offset];
}

void
plic_complete(int context, uint32 interrupt_id)
{
	int offset;
	int context_offset;

	if (context < 0 || 15871 < context)
		return;
	context_offset = 0x1000 / sizeof(uint32);

	offset = 0x200004 / sizeof(uint32);
	offset += context_offset * context;
	plic[offset] = interrupt_id;
}
