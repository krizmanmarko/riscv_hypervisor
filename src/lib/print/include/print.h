#ifndef PRINT_H
#define PRINT_H

void log(const char *fmt, ...);
void oops(const char *fmt, ...);	// used for non lethal issues
void panic(const char *fmt, ...);	// used for fatal issues

#endif // PRINT_H
