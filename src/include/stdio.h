#ifndef STDIO_H
#define STDIO_H

// void __noreturn__ panic(char *fmt, ...)
// ##__VA_ARGS__ -> ## removes comma if __VA_ARGS__ is empty
// TODO: test this monstrosity (was written in stage where i could not test it)
// TODO: does this create main rodata strings all sharing [PANIC...] prefix?
#define panic(fmt, ...) \
	do { \
		printf("\033[38;2;255;0;0m[PANIC(%d) %s:%d]: ", \
			get_hartid(), \
			__FILE__, \
			__LINE__ \
		); \
		printf(fmt, ##__VA_ARGS__); \
		while (1); \
	} while (0);

void printf(const char *fmt, ...);
void init_printf();

#endif // STDIO_H
