#include <defs.h>

// idea of this device is to multiplex plic accesses between virtual machines
// claim/complete and thresholds just have to be correctly mapped
// but other plic registers actually need hypervisor intervention
// luckily guests only setup these registers once so this shouldn't effect perf
// VM hartid always starts at 0

// From RISC-V privileged specification
//    Hart IDs might not necessarily be numbered contiguously in a
//    multiprocessor system, but at least one hart must have a hart ID of zero.

// this is how it looks
