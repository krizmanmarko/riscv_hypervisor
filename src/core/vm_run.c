// TODO: use VM config struct

#include "defs.h"
#include "dtb.h"
#include "memory.h"
#include "riscv.h"
#include "stdio.h"
#include "string.h"

static void init_hs_pgtable();
static void init_hs();
static void init_vs();
void __attribute__((noreturn)) vm_run();

pte_t vm_pgtable[512 * 4] __attribute__((aligned(4 * PAGE_SIZE)));

static void
init_hs_pgtable()
{
	memset(vm_pgtable, '\x00', PAGE_SIZE * 4);	// make every entry invalid
	map_page(vm_pgtable, DTB_SERIAL, DTB_SERIAL, PTE_U | PTE_R | PTE_W);
	map_page(vm_pgtable, DTB_MEMORY, DTB_MEMORY + FIRMWARE_SIZE + 0x11000, PTE_U | PTE_R | PTE_W | PTE_X);
}

static void
init_hs()
{
	CSRW(hstatus, HSTATUS_VSXL | HSTATUS_SPV);
	CSRW(hedeleg, 0ULL);
	CSRW(hideleg, HIDELEG_VSTI);
	CSRW(hvip, 0ULL);
	CSRW(hip, 0ULL);
	CSRW(hie, 0ULL);
	// hgeip is RO
	CSRW(hgeie, 0ULL);
	CSRW(hcounteren, HCOUNTEREN_TM);	// vm can now read time
	CSRW(htimedelta, 0ULL);
	CSRW(htval, 0ULL);
	CSRW(htinst, 0ULL);

	init_hs_pgtable();
	CSRW(hgatp, ATP_MODE_SV39 | ((KPA2PA((uint64) vm_pgtable)) >> 12));
	asm volatile("hfence.gvma");	// TODO: needed?
}

static void
init_vs()
{
	CSRW(vsstatus, 0ULL);
	CSRW(vsip, 0ULL);
	CSRW(vsie, 0ULL);
	CSRW(vstvec, 0ULL);
	CSRW(vsscratch, 0ULL);
	CSRW(vsepc, 0ULL);
	CSRW(vscause, 0ULL);
	CSRW(vstval, 0ULL);
	CSRW(vsatp, 0ULL);
}

void __attribute__((noreturn))
vm_run()
{
	init_vs();
	init_hs();

	//CSRS(scounteren, SCOUNTEREN_TM);	// enable U-mode to access time

	// prepare for sret
	CSRS(sstatus, SSTATUS_SPP);
	CSRW(sepc, 0x80000000ULL);
	//vm_entry();
	while (1);	// this and noreturn removes function epilogue
}

