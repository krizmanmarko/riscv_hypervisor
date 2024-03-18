#include "defs.h"
#include "riscv.h"
#include "string.h"
#include "vcpu.h"
#include "vm_config.h"

void
init_vcpu(struct vm_config *conf)
{
	memset(&conf->vcpu, '\x00', sizeof(struct vcpu));
	CSRW(sscratch, &conf->vcpu);
}
