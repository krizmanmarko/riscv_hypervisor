#ifndef DEFS_H
#define DEFS_H

#include "types.h"

// {lib,test}/* should be seperated

// boot.S
void boot(uint64 hartid, void *dtb);

// driver/cpu.c
uint64 hartid();
void interrupt_enable();
void interrupt_disable();
void push_int_disable();
void pop_int_disable();
int interrupt_status();
void hart_init_cpu();

// driver/uart.c
void uartputc(char c);
void init_uart();

// kmem.c
void *kmalloc();
void kfree(void *pa);
void init_kmem();

// linkerscript.c
extern char text[];
extern char etext[];
extern char rodata[];
extern char erodata[];
extern char data[];
extern char edata[];
extern char end[];

// machine.c
void init_machine();

// main.c
int main();

// trap.c
void hs_interrupt_handler(uint64 scause);
void hs_exception_handler(uint64 scause);

// trap.S
void hstrapvec();
void mtrapvec();

// vmem.c
pte_t *walk(pte_t *pgtable, uint64 va, int alloc);
void init_vmem();

#endif // DEFS_H
