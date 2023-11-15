#ifndef STDIO_H
#define STDIO_H

//#include "defs.h"

// void __noreturn__ panic(char *fmt, ...)
// ##__VA_ARGS__ -> ## removes comma if __VA_ARGS__ is empty
// TODO: test this monstrosity (was written in stage where i could not test it)
// TODO: does this create main rodata strings all sharing [PANIC...] prefix?
#define panic(fmt, ...) \
	do { \
		printf("[PANIC(%d) %s:%d]: " fmt, \
			hartid(), \
			__FILE__, \
			__LINE__, \
			##__VA_ARGS__ \
		); \
		while (1); \
	} while (0);

void printf(char *fmt, ...);
void init_printf();

#endif // STDIO_H
