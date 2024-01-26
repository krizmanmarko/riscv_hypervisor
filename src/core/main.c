#include "defs.h"
#include "dtb.h"
#include "lock.h"
#include "stdio.h"
#include "sbi.h"
#include "riscv.h"

void __attribute__((noreturn)) main();

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

	vm_run();
}
