#include "defs.h"
#include "dtb.h"
#include "riscv_hypervisor.h"
#include "stdio.h"
#include "test.h"

int finished_init = 0;

int
main()
{
	if (hartid() == 0) {
		init_uart();
		init_printf();
		printf("Booting!\n");
		hart_init_cpu();
		init_kmem();
		init_vmem();

		// Testing purposes
		//exc_instruction_address_misaligned_test();
		//exc_instruction_access_fault_test();
		//exc_illegal_instruction_test();
		//exc_breakpoint_test();
		//exc_load_address_misaligned_test();
		//exc_load_access_fault_test();
		//exc_store_or_amo_address_misaligned_test();
		//exc_store_or_amo_access_fault_test();

		extern char etext[];	// linker script
		printf("%p\n", etext);
		// End testing purposes

		finished_init = 1;
	} else {
		hart_init_cpu();
	}
	while (!finished_init);

	while (1);
}
