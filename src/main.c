#include "riscv_hypervisor.h"
#include "dtb.h"

extern void test();
int main();

int
main()
{
	test();
	while (1);
}
