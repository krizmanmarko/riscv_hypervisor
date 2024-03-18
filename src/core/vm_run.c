// TODO: use VM config struct

#include "defs.h"
#include "dtb.h"
#include "memory.h"
#include "riscv.h"
#include "stdio.h"
#include "string.h"
#include "vcpu.h"
#include "vm_config.h"

static void init_hs_pgtable();
static void init_hs();
static void init_vs();
void __attribute__((noreturn)) vm_run();

static void
init_hs_pgtable(struct vm_config *conf)
{
	memset(conf->vm_pgtable, '\x00', PAGE_SIZE * 4);

	// memory
	map_pages(
		conf->vm_pgtable,
		conf->memory_base,	// va
		conf->image_base,	// pa
		conf->memory_size,	//size
		PTE_U | PTE_R | PTE_W | PTE_X
	);

	// devices
	map_page(conf->vm_pgtable, DTB_SERIAL, DTB_SERIAL, PTE_U | PTE_R | PTE_W);
}

static void
init_hs(struct vm_config *conf)
{
	CSRW(hstatus, HSTATUS_VSXL | HSTATUS_SPV);
	CSRW(hedeleg, 0ULL);
	CSRW(hideleg, HIDELEG_VSTI);
	CSRW(hvip, 0ULL);
	CSRW(hip, 0ULL);
	CSRW(hie, 0ULL);
	CSRW(hgeie, 0ULL);
	CSRW(hcounteren, HCOUNTEREN_TM);	// vm can now read time
	CSRW(htimedelta, 0ULL);
	CSRW(htval, 0ULL);
	CSRW(htinst, 0ULL);

	init_hs_pgtable(conf);
	CSRW(hgatp, ATP_MODE_SV39 | ((KVA2PA((uint64) conf->vm_pgtable)) >> 12));
	__asm__ volatile("hfence.gvma");
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
	CSRW(vstimecmp, -1LL);
}

static struct vm_config *
get_config(uint64 hartid)
{
	for (int i = 0; i < config.nr_vms; i++)
		if (config.vm[i].cpu_affinity & (1 << hartid))
			return &config.vm[i];
	return (struct vm_config *)0;
}

void __attribute__((noreturn))
vm_run(uint64 hartid)
{
	struct vm_config *conf = get_config(hartid);

	init_vs();
	init_hs(conf);
	init_vcpu(conf);
	CSRS(sstatus, SSTATUS_SPP);
	CSRW(sepc, conf->entry);
	vm_enter();

	while (1);	// this and noreturn removes function epilogue
}

