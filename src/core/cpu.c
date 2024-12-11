#include <dtb.h>
#include <cpu.h>

__attribute__((__section__(".cpu_stacks")))
char cpu_stacks[DTB_NR_CPUS * CPU_STACK_SIZE];
