#include "riscv_hypervisor.h"
#include "dtb.h"
#include "types.h"

extern void test();
extern void init_printf();
extern uint64 hartid();
extern void init_cpu();

int main();

int finished_init = 0;

int
main()
{
	if (hartid() == 0) {
		init_uart();
		init_printf();
		init_cpu();
		finished_init = 1;
		// testing (1 cpu)
		test(0);
	} else {
		init_cpu();
	}

	while (!finished_init);
	test(2);
	while (1);
}

// 1 cpu
// init_uart
// init_printf

// all cpus
// init_cpu
