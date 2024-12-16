#include "print.h"

__attribute__((noreturn)) void
early_boot()
{
	// init uart: SBI handles it
	log("\033[0mBooting!");
	oops("%%p: %p", 0x63636363);
	panic("%%s: %s", "hello world %s");
	while (1);
	//mem prot
}
