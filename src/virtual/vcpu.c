#include "defs.h"
#include "riscv.h"
#include "stdio.h"
#include "string.h"
#include "types.h"
#include "vcpu.h"
#include "vm_config.h"

struct vcpu vcpus[DTB_NR_CPUS];

static struct vm_config *
get_config_for_cpu(uint64 hartid)
{
	for (int i = 0; i < nr_vms; i++)
		if (config[i].cpu_affinity & (1 << hartid))
			return &config[i];
	panic("No way (CPU configuration was already checked)");
}

// From RISC-V privileged specification
//    Hart IDs might not necessarily be numbered contiguously in a
//    multiprocessor system, but at least one hart must have a hart ID of zero.
static uint64
get_vhartid(uint64 hartid, uint64 cpu_affinity)
{
	uint64 vhartid = 0;
	for (uint64 i = 0; i < hartid; i++) {
		if (cpu_affinity & 1)
			vhartid++;
		cpu_affinity >>= 1;
	}
	return vhartid;
}

struct vcpu *
get_vcpu()
{
	return &vcpus[get_hartid()];
}

void
init_vcpu(struct vcpu *vcpu)
{
	uint64 hartid = get_hartid();

	vcpu->conf = get_config_for_cpu(hartid);
	memset(&vcpu->regs, '\x00', sizeof(vcpu->regs));
	vcpu->vhartid = get_vhartid(hartid, vcpu->conf->cpu_affinity);
	vcpu->last_claimed_irq_id = 0;
	vcpu->regs.x[10] = vcpu->vhartid;
	CSRW(sscratch, &vcpu->regs);
}
