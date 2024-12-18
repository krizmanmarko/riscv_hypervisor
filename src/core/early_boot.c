#include "print.h"

__attribute__((noreturn)) void
early_boot()
{
	// init uart: SBI handles it
	log("\033[0mBooting!");
	//mem prot
	while (1);
}
