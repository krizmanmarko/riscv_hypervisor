#ifndef DEFS_H
#define DEFS_H

#include "types.h"

// {lib,test}/* should be seperated

// linkerscript (phys)
extern char boottext[];
extern char eboottext[];
extern char bootdata[];
extern char ebootdata[];
extern char bsp[];
extern char ebsp[];
extern char eboot[];

// linkerscript (vas)
extern char text[];
extern char etext[];
extern char rodata[];
extern char erodata[];
extern char data[];
extern char edata[];
extern char dynamic[];

// boot.S
void boot(uint64 hartid, void *dtb);

// sboot.S
void setup(uint64 hartid, void *dtb);

// main.c
void __attribute__((noreturn)) main();

// kmem.c
void *kmalloc();
void init_kmem();

// driver/cpu.c
uint64 hartid();
void interrupt_enable();
void interrupt_disable();
void push_int_disable();
void pop_int_disable();
int interrupt_status();
void init_hart();

// trap.S
void hstrapvec();
void mtrapvec();

// TODO: FROM HERE IT IS OLD

// driver/uart.c
void uartputc(char c);
void init_uart();

// kmem.c
void *kmalloc();
void kfree(void *pa);
void init_kmem();

// machine.c
void init_machine();

// trap.c
void hs_interrupt_handler(uint64 scause);
void hs_exception_handler(uint64 scause);

// vmem.c
pte_t *walk(pte_t *pgtable, uint64 va, int alloc);
void init_vmem();

#endif // DEFS_H
