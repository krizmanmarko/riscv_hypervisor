#include "cpu.h"
#include "defs.h"
#include "dtb.h"
#include "lock.h"
#include "stdio.h"
#include "sbi.h"
#include "riscv.h"

#include "test.h"

extern int cpu_mapped;

void __attribute__((noreturn)) main();

static struct barrier bar = BARRIER_INITIALIZER(DTB_NR_CPUS);

void __attribute__((noreturn))
main(uint64 hartid)
{
	pte_t *pgtable;

	if (hartid == 0) {
		init_kmem();
		init_uart();
		printf("Booting!\n");
	}
	wait_barrier(&bar);
	pgtable = init_vmem();
	init_hart(pgtable);

	wait_barrier(&bar);	// final page tables must be active on all harts
	if (hartid == 0)
		cpu_mapped = 1;
	wait_barrier(&bar);	// do not allow any mycpu() call before toggling

	//vm_run();
	while (1);
}
