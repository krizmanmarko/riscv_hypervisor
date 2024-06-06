#include "cpu.h"
#include "defs.h"
#include "dtb.h"
#include "lock.h"
#include "stdio.h"
#include "sbi.h"
#include "riscv.h"

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

// TODO: testing (external interrupt passthrough)
	plic_set_priority(10, 1);
	// target supervisor mode
	plic_set_enabled(hartid*2 + 1, 10, 1);
	plic_set_threshold(hartid*2 + 1, 0);
	CSRS(hgeie, -1);
	CSRS(hie, -1);
	CSRS(sie, -1);
	CSRS(hideleg, -1);
// end testing

	vm_run(hartid);
	while (1);
}
