#include "defs.h"
#include "riscv.h"
#include "string.h"
#include "types.h"
#include "vcpu.h"
#include "vm_config.h"

#include "stdio.h"	// TODO: testing only

struct vcpu vcpus[DTB_NR_CPUS];

uint64
get_vhartid(struct vm_config *conf)
{
	uint64 tmp = conf->cpu_affinity;
	uint64 vhartid = 0;
	for (uint64 i = 0; i < get_hartid(); i++) {
		if (tmp & 1) {
			vhartid++;
		}
		tmp >>= 1;
	}
	return vhartid;
}


uint64
init_vcpu(struct vm_config *conf)
{
	struct vcpu *vcpu = &vcpus[get_hartid()];

	vcpu->vhartid = get_vhartid(conf);
	memset(&vcpu->regs, '\x00', sizeof(vcpu->regs));
	vcpu->regs.x[10] = vcpu->vhartid;
	CSRW(sscratch, &vcpu->regs);

	return vcpu->vhartid;
}
