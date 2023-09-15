#ifndef PANIC_H
#define PANIC_H

#include "types.h"

extern uint64 hartid();
extern void printf(char *fmt, ...);

// void __noreturn__ panic(char *s)
#define panic(s) 						\
	do { 							\
		printf(						\
			"[PANIC(%d) %s:%d]: %s\n",		\
			hartid(),				\
			__FILE__,				\
			__LINE__,				\
			s					\
		);						\
		while (1);					\
	} while (0);

#endif // PANIC_H
