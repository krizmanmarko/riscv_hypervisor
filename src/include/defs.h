#ifndef DEFS_H
#define DEFS_H

#include "types.h"

// {lib,test}/* should be seperated

// core/main.c
void __attribute__((noreturn)) main();

// core/sboot.S
void setup(uint64 hartid, void *dtb);

// core/trap.c
void hs_interrupt_handler(uint64 scause);
void hs_exception_handler(uint64 scause);

// core/trap.S
void hstrapvec();

// core/vm_run.c
void __attribute__((noreturn)) vm_run();

// driver/cpu.c
uint64 hartid();
void interrupt_enable();
void interrupt_disable();
void push_int_disable();
void pop_int_disable();
int interrupt_status();
void init_hart();

// driver/uart.c
void uartputc(char c);
void init_uart();

// mem/kmem.c
void *kmalloc();
void kfree(void *pa);
void init_kmem();

// mem/vmem.c
pte_t *walk(pte_t *pgtable, uint64 va, int alloc);
int map_page(pte_t *pgtable, uint64 va, uint64 pa, int pte_flags);
int map_pages(pte_t *pgtable, uint64 va, uint64 pa, uint64 size, int pte_flags);
void init_vmem();

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

#endif // DEFS_H
