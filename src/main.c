#include "defs.h"
#include "lock.h"
#include "stdio.h"

// temporary for testing
#include "test.h"
struct barrier b = BARRIER_INITIALIZER(3);

void __attribute__((noreturn))
main()
{
	if (hartid() == 0) {
		init_kmem();
		init_uart();
		// testing
		printf_test();
		// end testing
		printf("Booting!\n");
		init_vmem();
	}
	// testing
	spinlock_test();
	wait_barrier(&b);
	barrier_test();
	// end testing
	while (1);
}
