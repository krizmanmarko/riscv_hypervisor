// TODO: this whole file is just debugging
#include "bits.h"
#include "defs.h"
#include "riscv.h"
#include "types.h"
#include "sbi.h"
#include "stdio.h"

// WARNING!
// Locks do not disable interrupts. Do not use them in interrupt handlers unless
// you know what you are doing. This also means no printf()

void
hs_interrupt_handler(uint64 scause)
{
// handling timers
// sstatus.sie
// sie.stie
// sbi_set_timer
// 	timer 1 sekunda -> 10**7

	scause = scause << 1 >> 1;
	if (scause == INT_SUPERVISOR_TIM)
		sbi_set_timer(CSRR(time) + 10000000);
	else if (scause == INT_SUPERVISOR_EXT) {
		uint32 id = plic_claim(1);
		printf("grabbed %u\n", ((char *)DTB_SERIAL)[0]);
		plic_complete(1, id);
	}
}

static int
is_access_to_plic()
{
	// TODO: PLIC can be mapped anywhere, use config.device struct
	//       to check correctly
	uint64 addr = CSRR(stval);
	if (addr < DTB_PLIC || DTB_PLIC + DTB_PLIC_SIZE < addr)
		return 0;
	return 1;
}

void
hs_exception_handler(uint64 scause)
{
	switch (scause) {
	case EXC_ECALL_FROM_VS:
		//TODO: testing
		//printf("hello from exception %u\n", scause);
		CSRW(vstimecmp, CSRR(time) + 10000000);
		CSRW(sepc, CSRR(sepc) + 4);
		break;
	case EXC_LOAD_GUEST_PAGE_FAULT:
		if (is_access_to_plic())
			vplic_handle_load_page_fault();
		else
			panic("Bad read access\n");
		break;
	case EXC_STORE_OR_AMO_GUEST_PAGE_FAULT:
		if (is_access_to_plic())
			vplic_handle_store_or_amo_page_fault();
		else
			panic("Bad write access\n");
		break;
	default:
		panic("unknown exception %d\n", scause);
	}
}

/*
I: imm[12] rs1[5] funct3[3] rd[5] opcode[7]
lb	Load Byte	I	0000011	0x0	rd = M[rs1+imm][0:7]
lh	Load Half	I	0000011	0x1	rd = M[rs1+imm][0:15]
lw	Load Word	I	0000011	0x2	rd = M[rs1+imm][0:31]
lbu	Load Byte (U)	I	0000011	0x4	rd = M[rs1+imm][0:7]	zero-extends
lhu	Load Half (U)	I	0000011	0x5	rd = M[rs1+imm][0:15] zero-extends

S: imm[7] rs2[5] rs1[5] funct3 imm[4:0] opcode
sb	Store Byte 	S	0100011	0x0	M[rs1+imm][0:7]	= rs2[0:7]
sh	Store Half 	S	0100011	0x1	M[rs1+imm][0:15] = rs2[0:15]
sw	Store Word 	S	0100011	0x2	M[rs1+imm][0:31] = rs2[0:31]
*/
