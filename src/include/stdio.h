#ifndef STDIO_H
#define STDIO_H

#include "defs.h"

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

void printf(char *fmt, ...);
void init_printf();

#endif // STDIO_H
