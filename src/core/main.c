#include "defs.h"
#include "dtb.h"
#include "lock.h"
#include "stdio.h"
#include "sbi.h"
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
	
	sbi_set_timer(10000000000000);	// 2 sekundi
	extern void vm_run();
	vm_run();
	while (1);
}
