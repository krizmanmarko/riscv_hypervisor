#include "riscv_hypervisor.h"
#include "dtb.h"

extern void test();
extern void init_printf();

int main();

int
main()
{
	init_printf();

	// testing
	test();
	while (1);
}
