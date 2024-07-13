#include "bits.h"
#include "defs.h"
#include "riscv.h"
#include "stdio.h"
#include "types.h"

// TODO: make use of device struct (irq_phys -> irq_virt, hartid -> vhartid)

// idea of this device is to multiplex plic accesses between virtual machines
// claim/complete and thresholds just have to be correctly mapped
// but other plic registers actually need hypervisor intervention
// luckily guests only setup these registers once so this shouldn't effect perf
// VM hartid always starts at 0

// From RISC-V privileged specification
//    Hart IDs might not necessarily be numbered contiguously in a
//    multiprocessor system, but at least one hart must have a hart ID of zero.


static int
allow_word_access_to_priority_regs(uint64 offset, unsigned int irq_id)
{
	if (offset == irq_id * 4)
		return 1;
	return 0;
}

static int
allow_word_access_to_pending_regs(uint64 offset, unsigned int irq_id)
{
	if (offset == 0x1000 + irq_id / 32)
		return 1;
	return 0;
}

static int
allow_word_access_to_enable_regs(uint64 offset, uint64 context)
{
	uint64 lowest, highest;
	lowest = 0x2000 + context * 0x80;
	highest = lowest + 0x80;
	if (lowest <= offset && offset < highest)
		return 1;
	return 0;
}

static int
allow_word_access_to_threshold_reg(uint64 offset, uint64 context)
{
	if (offset == 0x200000 + context * 0x1000)
		return 1;
	return 0;
}

static int
allow_word_access_to_claim_complete_reg(uint64 offset, uint64 context)
{
	if (offset == 0x200004 + context * 0x1000)
		return 1;
	return 0;
}

// TODO: test this function
// returns 32 bit mask or negative if claim/complete was accessed
static int64
get_restricted_mask(uint64 offset, int vcontext, struct mmio_dev *dev)
{
	if (dev->irq_virt == 0) {	// 0 - meaning no interrupts
		return 0;
	} else if (allow_word_access_to_priority_regs(offset, dev->irq_virt)) {
		printf("allowing access to priority regs\n");
		return 0xffffffff;
	} else if (allow_word_access_to_pending_regs(offset, dev->irq_virt)) {
		printf("allowing access to pending regs\n");
		return 1 << dev->irq_virt % 32;
	} else if (allow_word_access_to_enable_regs(offset, vcontext)) {
		printf("allowing access to enable regs\n");
		return 0xffffffff;
	} else if (allow_word_access_to_threshold_reg(offset, vcontext)) {
		printf("allowing access to threshold reg\n");
		return 0xffffffff;
	} else if (allow_word_access_to_claim_complete_reg(offset, vcontext)) {
		printf("faking access to claim/complete reg\n");
		return 0x8000000000000000;
	}
	return 0;
}

static int64
verify_access(uint64 offset, int vcontext, struct mmio_dev **devs)
{
	int64 mask;
	struct mmio_dev *dev;

	mask = 0;
	dev = devs[0];
	for (int i = 0; i < MAX_DEVICES && dev != 0; dev = devs[++i])
		mask |= get_restricted_mask(offset, vcontext, dev);
	return mask;
}

// lwu zero extends 32-bit value and stores into 64-bit register
static void
try_emulate_lwu(struct vcpu *vcpu, uint8 rd, uint64 addr)
{
	int64 mask;	// restricted view (for interrupt pending bits)

	// TODO: USE config struct for dtb_plic
	mask = verify_access(
		addr - DTB_PLIC,
		2 * vcpu->vhartid + 1,
		vcpu->conf->devices
	);

	if (mask < 0) {
		// fake access to claim register
		vcpu->regs.x[rd] = vcpu->last_claimed_irq_id;
	} else {
		// execute restricted load
		vcpu->regs.x[rd] = mask & *(uint32 *)addr;
	}
}

// lw sign extends 32-bit value and stores into 64-bit register
static void
try_emulate_lw(struct vcpu *vcpu, uint8 rd, uint64 addr)
{
	try_emulate_lwu(vcpu, rd, addr);
	// add sign extension
	if (vcpu->regs.x[rd] & 0x80000000)
		vcpu->regs.x[rd] |= 0xffffffff00000000;
}

static void
try_emulate_load(struct vcpu *vcpu, uint64 addr)
{
	uint64 htinst;
	uint8 opcode, rd, funct3, addr_offset;

	htinst = CSRR(htinst);	// 0... addr_offset[5] funct3[3] rd[5] opcode[7]

	opcode = get_value(htinst, 0, 7) | 2;	// replacing bit 1 by 1
	rd = get_value(htinst, 7, 5);
	funct3 = get_value(htinst, 12, 3);
	// this is non zero only on misaligned access -> always zero in my case
	addr_offset = get_value(htinst, 15, 5);

	if (opcode != 0x3)
		panic("Bad opcode for load");
	if (addr_offset != 0)
		panic("Misaligned access with C extension???");

	switch (funct3) {
	case 2:
		try_emulate_lw(vcpu, rd, addr);
		break;
	case 6:
		try_emulate_lwu(vcpu, rd, addr);
		break;
	default:
		panic("Bad funct3 for load");
	}

	// TODO: let's hope this is the only difference when using compressed
	// For a standard compressed instruction (16-bit size), the transformed
	// instruction is found as follows:
	//   1. Expand the compressed instruction to its 32-bit equivalent.
	//   2. Transform the 32-bit equivalent instruction.
	//   3. Replace bit 1 with a 0.
	if (CSRR(htinst) & 2)
		CSRW(sepc, CSRR(sepc) + 4);	// uncompressed load
	else
		CSRW(sepc, CSRR(sepc) + 2);	// compressed load
}

void
vplic_handle_load_page_fault()
{
	uint64 addr;
	struct vcpu *vcpu;

	addr = CSRR(stval);
	vcpu = &vcpus[get_hartid()];
	try_emulate_load(vcpu, addr);
}

// lwu zero extends 32-bit value and stores into 64-bit register
static void
try_emulate_sw(struct vcpu *vcpu, uint8 rs2, uint64 addr)
{
	int64 mask;	// restricted view (for interrupt pending bits)

	// TODO: USE config struct for dtb_plic
	mask = verify_access(
		addr - DTB_PLIC,
		2 * vcpu->vhartid + 1,
		vcpu->conf->devices
	);

	if (mask < 0) {		// this access is plic_complete
		// complete with wrong irq_id is ignored
		if (vcpu->last_claimed_irq_id == vcpu->regs.x[rs2]) {
			vcpu->last_claimed_irq_id = 0;
			CSRC(hvip, HVIP_VSEIP);
			mask = 0xffffffff;
		}
	}
	// execute restricted store or plic_complete
	*(uint32 *)addr = mask & vcpu->regs.x[rs2];
}

static void
try_emulate_store_or_amo(struct vcpu *vcpu, uint64 addr)
{
	// how does amo look???? -> check aq and rl bits and exit if set
	uint64 htinst;
	uint8 opcode, funct3, addr_offset, rs2;

	htinst = CSRR(htinst);
	opcode = get_value(htinst, 0, 7) | 2;	// replacing bit 1 by 1
	funct3 = get_value(htinst, 12, 3);
	addr_offset = get_value(htinst, 15, 5);
	rs2 = get_value(htinst, 20, 5);
	if ((get_value(htinst, 7, 5) | get_value(htinst, 25, 7)) != 0)
		panic("VPLIC does not support AMO");

	if (opcode != 0x23)
		panic("Bad opcode for store");
	if (addr_offset != 0)
		panic("Misaligned access with C extension???");

	if (funct3 == 2)
		try_emulate_sw(vcpu, rs2, addr);
	else
		panic("Bad funct3 for store");

	if (CSRR(htinst) & 2)
		CSRW(sepc, CSRR(sepc) + 4);	// uncompressed store / amo
	else
		CSRW(sepc, CSRR(sepc) + 2);	// compressed store
}

void
vplic_handle_store_or_amo_page_fault()
{
	uint64 addr;
	struct vcpu *vcpu;

	addr = CSRR(stval);
	vcpu = &vcpus[get_hartid()];
	try_emulate_store_or_amo(vcpu, addr);
}

/*
NOT COMPRESSED
83008100	lb	ra, 8(sp)
83408100	lbu	ra, 8(sp)
83108100	lh	ra, 8(sp)
83508100	lhu	ra, 8(sp)
83208100	lw	ra, 8(sp)
83608100	lwu	ra, 8(sp)
83308100	ld	ra, 8(sp)
23041100	sb	ra, 8(sp)
23141100	sh	ra, 8(sp)
23241100	sw	ra, 8(sp)
23341100	sd	ra, 8(sp)
13000000	nop

COMPRESSED
83008100	lb	ra, 8(sp)
83408100	lbu	ra, 8(sp)
83108100	lh	ra, 8(sp)
83508100	lhu	ra, 8(sp)
a240		lw	ra, 8(sp)
83608100	lwu	ra, 8(sp)
a260		ld	ra, 8(sp)
23041100	sb	ra, 8(sp)
23141100	sh	ra, 8(sp)
06c4		sw	ra, 8(sp)
06e4		sd	ra, 8(sp)
0100		nop

*/
