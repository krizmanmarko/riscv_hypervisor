#include "string.h"
#include "vcpu.h"

// this is not necessarily equal to DTB_NR_CPUS
// TODO: this value is obtained from VM config struct
#define NR_VMS 1

struct vcpu vcpus[NR_VMS];

void
init_vcpu(int vm_id)
{
	memset(vcpus[vm_id], '\x00', sizeof(struct vcpu));
}
