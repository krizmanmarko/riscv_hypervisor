#include "printf.h"

__attribute__((noreturn)) void
early_boot()
{
	// init uart: SBI handles it
	printf("Booting!\n");
	while (1);
	//mem prot
}
