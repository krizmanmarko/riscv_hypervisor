#ifndef TYPES_H
#define TYPES_H

#ifndef __ASSEMBLER__
typedef signed char int8;
typedef signed short int16;
typedef signed int int32;
typedef signed long int64;

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long uint64;

typedef uint64 pte_t;

// linked list
struct list {
	struct list *next;
};

#endif // __ASSEMBLER__
#endif // TYPES_H
