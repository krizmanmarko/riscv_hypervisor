#ifndef DEFS_H
#define DEFS_H

#include "types.h"
#include "vm_config.h"

// {lib,test}/* should be seperated

// core/main.c
void __attribute__((noreturn)) main();

// core/sboot.S
void setup(uint64 hartid, void *dtb);
void relocate_stack(uint64 offset);

// core/trap.c
void hs_interrupt_handler(uint64 scause);
void hs_exception_handler(uint64 scause);

// core/trap.S
void hstrapvec();
void vm_enter();

// core/vcpu.c
void init_vcpu(struct vm_config *conf);

// core/vm_run.c
void __attribute__((noreturn)) vm_run(uint64 hartid);

// driver/cpu.c
struct cpu *mycpu();
uint64 get_hartid();
void init_hart(pte_t *pgtable);

// driver/uart.c
void uartputc(char c);
void init_uart();

// driver/plic.c
void plic_set_priority(int interrupt_source, uint32 priority);
void plic_set_enabled(int interrupt_source, int context, int enable);
void plic_set_threshold(int context, uint32 threshold);
uint32 plic_claim(int context);
void plic_complete(int context, uint32 interrupt_id);

// mem/kmem.c
void *kmalloc();
void kfree(void *pa);
void init_kmem();

// mem/vmem.c
pte_t *walk(pte_t *pgtable, uint64 va, int alloc);
int map_page(pte_t *pgtable, uint64 va, uint64 pa, int pte_flags);
int map_pages(pte_t *pgtable, uint64 va, uint64 pa, uint64 size, int pte_flags);
pte_t *init_vmem();

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
extern char cpu_structs[];
extern char ecpu_structs[];
extern char dynamic[];

#endif // DEFS_H
