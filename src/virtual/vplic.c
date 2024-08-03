#include "bits.h"
#include "defs.h"
#include "memory.h"
#include "riscv.h"
#include "stdio.h"
#include "types.h"
#include "emulate.h"

static int
is_access_to_priority_reg(uint64 offset)
{
	if (offset < 0x1000)	// uint always >= 0
		return 1;
	return 0;
}

static int
is_access_to_pending_regs(uint64 offset)
{
	if (0x1000 <= offset && offset < 0x1080)
		return 1;
	return 0;
}

static int
is_access_to_enable_regs(uint64 offset)
{
	if (0x2000 <= offset && offset < 0x200000)
		return 1;
	return 0;
}

static int
is_access_to_threshold_reg(uint64 offset)
{
	for (unsigned int ctx = 0; ctx < 15872; ctx++)
		if (0x200000 + ctx * 0x1000 == offset)
			return 1;
	return 0;
}

static int
is_access_to_claim_complete_reg(uint64 offset)
{
	for (unsigned int ctx = 0; ctx < 15872; ctx++)
		if (0x200004 + ctx * 0x1000 == offset)
			return 1;
	return 0;
}

static void
parse_htinst(struct instr *instr)
{
	uint64 htinst = CSRR(htinst);

	instr->opcode = get_value(htinst, 0, 7);
	instr->rd = get_value(htinst, 7, 5);
	instr->funct3 = get_value(htinst, 12, 3);
	instr->addr_offset = get_value(htinst, 15, 5);
	instr->rs2 = get_value(htinst, 20, 5);

	if (instr->addr_offset != 0)
		panic("Misaligned access with C extension???");
	if (get_value(htinst, 25, 7) != 0)
		panic("VPLIC does not support this instruction (AMO?)");
}

static int
virt_to_phys_irq(struct mmio_dev **devs, int virt_irq)
{
	for (int i = 0; i < MAX_DEVS && devs[i] != 0; i++)
		if (virt_irq == devs[i]->virt_irq)
			return devs[i]->phys_irq;
	return 0;
}

static int
phys_to_virt_irq(struct mmio_dev **devs, int phys_irq)
{
	for (int i = 0; i < MAX_DEVS && devs[i] != 0; i++)
		if (phys_irq == devs[i]->phys_irq)
			return devs[i]->virt_irq;
	return 0;
}

static uint32
handle_priority_reg_load(struct mmio_dev **devs, uint64 offset)
{
	int virt_irq, phys_irq;

	virt_irq = offset / 4;
	phys_irq = virt_to_phys_irq(devs, virt_irq);
	return plic_get_priority(phys_irq);
}

static uint32
handle_pending_regs_load(struct mmio_dev **devs, uint64 offset)
{
	int virt_irq, phys_irq;
	int virt_irq_group;	// 32 bits together
	uint32 masked;

	virt_irq_group = (offset - 0x1000) / 4;
	masked = 0;
	for (int i = 0; i < 32; i++) {
		virt_irq = virt_irq_group * 32 + i;
		phys_irq = virt_to_phys_irq(devs, virt_irq);
		if (plic_get_pending(phys_irq))
			masked |= 1 << i;	// set virt_irq in this group
	}
	return masked;
}

static uint32
handle_enable_regs_load(struct mmio_dev **devs,
			uint64 offset,
			uint64 vctx,
			uint64 pctx)
{
	int virt_irq, phys_irq;
	int virt_irq_group;	// 32 bits together
	uint32 masked;

	// bad access for this hart's vctx
	if (vctx != (offset - 0x2000) / 0x80)
		return 0;
	virt_irq_group = (offset - 0x2000 - vctx * 0x80) / 4;
	masked = 0;
	for (int i = 0; i < 32; i++) {
		virt_irq = virt_irq_group * 32 + i;
		phys_irq = virt_to_phys_irq(devs, virt_irq);
		if (plic_get_enabled(pctx, phys_irq))
			masked |= 1 << i;	// set virt_irq in this group
	}
	return masked;
}

static uint32
handle_threshold_reg_load(uint64 offset, uint64 vctx, uint64 pctx)
{
	// bad access for this hart's vctx
	if (vctx != (offset - 0x200000) / 0x1000)
		return 0;
	return plic_get_threshold(pctx);
}

static uint32
handle_claim_reg_load(struct mmio_dev **devs, uint64 offset, uint64 vctx)
{
	if (vctx != (offset - 0x200004) / 0x1000)
		return 0;
	// do not ACTUALLY plic_claim, just fake it
	return phys_to_virt_irq(devs, get_vcpu()->last_claimed_irq_id);
}

static void
emulate_load(struct instr *instr)
{
	struct vcpu *vcpu;
	struct mmio_dev **devs;
	uint64 offset, vctx, pctx;
	uint32 val;

	vcpu = get_vcpu();
	devs = vcpu->conf->devices;
	offset = instr->addr - DTB_PLIC;
	vctx = 2 * vcpu->vhartid + 1;
	pctx = 2 * get_hartid() + 1;
	if (is_access_to_priority_reg(offset)) {
		val = handle_priority_reg_load(devs, offset);
	} else if (is_access_to_pending_regs(offset)) {
		val = handle_pending_regs_load(devs, offset);
	} else if (is_access_to_enable_regs(offset)) {
		val = handle_enable_regs_load(devs, offset, vctx, pctx);
	} else if (is_access_to_threshold_reg(offset)) {
		val = handle_threshold_reg_load(offset, vctx, pctx);
	} else if (is_access_to_claim_complete_reg(offset)) {
		val = handle_claim_reg_load(devs, offset, vctx);
	} else {
		panic("Bad PLIC access");
	}

	vcpu->regs.x[instr->rd] = val;
	if (instr->funct3 == 2 && val & 0x80000000)	// lw (signed load)
		vcpu->regs.x[instr->rd] |= 0xffffffff00000000;

	if (instr->opcode & 2)
		CSRW(sepc, CSRR(sepc) + 4);	// uncompressed load
	else
		CSRW(sepc, CSRR(sepc) + 2);	// compressed load
}

void
vplic_handle_load_page_fault(uint64 addr)
{
	struct instr instr;
	parse_htinst(&instr);
	if ((instr.opcode | 2) != 0x3)	// replacing bit 1 with 1 (isa 8.6.3)
		panic("Bad opcode for load");
	if (instr.funct3 != 2 && instr.funct3 != 6)
		panic("Bad funct3 for load");

	instr.addr = addr;
	emulate_load(&instr);
}

static void
handle_priority_reg_store(struct mmio_dev **devs, uint64 offset, uint32 val)
{
	int virt_irq, phys_irq;

	virt_irq = offset / 4;
	phys_irq = virt_to_phys_irq(devs, virt_irq);
	plic_set_priority(phys_irq, val);
}

static void
handle_enable_regs_store(struct mmio_dev **devs,
			 uint64 offset,
			 uint32 val,
			 uint64 vctx,
			 uint64 pctx)
{
	int virt_irq, phys_irq;
	int virt_irq_group;	// 32 bits together

	// bad access for this hart's vctx
	if (vctx != (offset - 0x2000) / 0x80)
		return;
	virt_irq_group = (offset - 0x2000 - vctx * 0x80) / 4;
	for (int i = 0; i < 32; i++) {
		virt_irq = virt_irq_group * 32 + i;
		phys_irq = virt_to_phys_irq(devs, virt_irq);
		plic_set_enabled(pctx, phys_irq, 1 << i & val);
	}
}

static void
handle_threshold_reg_store(uint64 offset, uint64 vctx, uint64 pctx, uint64 val)
{
	if (vctx != (offset - 0x200000) / 0x1000)
		return;
	return plic_set_threshold(pctx, val);
}

static void
handle_complete_reg_store(struct vcpu *vcpu,
			  uint64 offset,
			  uint64 vctx,
			  uint64 pctx,
			  uint32 val)
{
	uint32 phys_irq;
	if (vctx != (offset - 0x200004) / 0x1000)
		return;
	phys_irq = virt_to_phys_irq(vcpu->conf->devices, val);
	if (phys_irq == vcpu->last_claimed_irq_id) {
		plic_complete(pctx, phys_irq);
		vcpu->last_claimed_irq_id = 0;
		CSRC(hvip, HVIP_VSEIP);
	}
}

static void
emulate_store(struct instr *instr)
{
	struct vcpu *vcpu;
	struct mmio_dev **devs;
	uint64 offset, vctx, pctx;
	uint32 val;

	vcpu = get_vcpu();
	devs = vcpu->conf->devices;
	offset = instr->addr - DTB_PLIC;
	vctx = 2 * vcpu->vhartid + 1;
	pctx = 2 * get_hartid() + 1;
	val = vcpu->regs.x[instr->rs2];
	if (is_access_to_priority_reg(offset)) {
		handle_priority_reg_store(devs, offset, val);
	} else if (is_access_to_pending_regs(offset)) {
		// nop (I am guessing pending is read only)
	} else if (is_access_to_enable_regs(offset)) {
		handle_enable_regs_store(devs, offset, val, vctx, pctx);
	} else if (is_access_to_threshold_reg(offset)) {
		handle_threshold_reg_store(offset, vctx, pctx, val);
	} else if (is_access_to_claim_complete_reg(offset)) {
		handle_complete_reg_store(vcpu, offset, vctx, pctx, val);
	} else {
		panic("Bad PLIC access");
	}

	// no need to edit VM's regs

	if (instr->opcode & 2)
		CSRW(sepc, CSRR(sepc) + 4);	// uncompressed store
	else
		CSRW(sepc, CSRR(sepc) + 2);	// compressed store
}

void
vplic_handle_store_or_amo_page_fault(uint64 addr)
{
	struct instr instr;
	parse_htinst(&instr);

	if ((instr.opcode | 2) != 0x23)	// replacing bit 1 with 1 (isa 8.6.3)
		panic("Bad opcode for store");
	if (instr.funct3 != 2)
		panic("Bad funct3 for store");

	instr.addr = addr;
	emulate_store(&instr);
}

// injects interrupt to VS-level if plic_claim is successful
// WARNING! -> do not use locks in interrupt (does not apply to exceptions)
void
vplic_handle_interrupt()
{
	uint64 hartid = get_hartid();
	struct vcpu *vcpu = get_vcpu();
	vcpu->last_claimed_irq_id = plic_claim(2 * hartid + 1);
	if (vcpu->last_claimed_irq_id != 0)
		CSRS(hvip, HVIP_VSEIP);
}
