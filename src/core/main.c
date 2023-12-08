#include "defs.h"
#include "dtb.h"
#include "lock.h"
#include "stdio.h"
#include "riscv.h"

struct barrier bar = BARRIER_INITIALIZER(DTB_NR_CPUS);

void __attribute__((noreturn))
main()
{
	if (hartid() == 0) {
		init_kmem();
		init_uart();
		printf("Booting!\n");
		init_vmem();
	}
	wait_barrier(&bar);
	init_hart();
	extern void vm_run();
	printf("%p\n", CSRR(sepc));
	vm_run();
	while (1);
}
