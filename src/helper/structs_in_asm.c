// this section is interesting
// it is needed to get struct offset and size information into assembly files
// https://stackoverflow.com/questions/8259447/give-structure-offset-attribute-to-assembler

#include <stddef.h>
#include "cpu.h"
#include "lock.h"

#define DEFINE(sym, val) \
	__asm__ volatile("\n-> " #sym " %0 " #val "\n" : : "i" (val))

#define DEFINE_OFFSET(sym, struct, field) \
	DEFINE(sym, offsetof(struct, field));

#define DEFINE_SIZE(sym, type) \
	DEFINE(sym, sizeof(type));

void
struct_defines()
{
	// cpu.h
	DEFINE_SIZE(CPU_SIZE, struct cpu);
	DEFINE_OFFSET(CPU_HARTID_OFF, struct cpu, hartid);
	DEFINE_OFFSET(CPU_STACK_OFF, struct cpu, stack);
	DEFINE_SIZE(CPU_STACK_SIZE, ((struct cpu *)0)->stack);

	// lock.h
	DEFINE_OFFSET(LOCK_LOCKED_OFF, struct lock, locked);
}
