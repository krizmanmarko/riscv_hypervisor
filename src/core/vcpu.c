#include "defs.h"
#include "riscv.h"
#include "string.h"
#include "vcpu.h"

// this is not necessarily equal to DTB_NR_CPUS
// TODO: this value is obtained from VM config struct
#define NR_VMS 1

struct vcpu vcpus[NR_VMS];

void
init_vcpu(int id)
{
	memset(&vcpus[id], '\x00', sizeof(struct vcpu));
}

void
activate_vcpu(int id)
{
	CSRW(sscratch, &vcpus[id]);
	vm_enter();
}
