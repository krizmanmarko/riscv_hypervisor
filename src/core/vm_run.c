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

	// map vm image
	map_pages(
		conf->vm_pgtable,
		conf->memory_base,	// va
		conf->image_base,	// pa
		conf->image_size,	// size
		PTE_U | PTE_R | PTE_W | PTE_X
	);

	// map leftover memory

	// devices
	// xv6 uses these MMIO devices:
	// kvmmap(kpgtbl, UART0, UART0, PGSIZE, PTE_R | PTE_W);
	// kvmmap(kpgtbl, VIRTIO0, VIRTIO0, PGSIZE, PTE_R | PTE_W);
	// kvmmap(kpgtbl, PLIC, PLIC, 0x400000, PTE_R | PTE_W);

// TODO: testing (external interrupt passthrough)
// WARNING: you should not initialize PLIC, just multiplex it correctly
	int hartid = 0;
	plic_set_priority(10, 1);
	// target supervisor mode
	plic_set_enabled(hartid*2 + 1, 10, 1);
	plic_set_threshold(hartid*2 + 1, 0);
// end testing

	// TODO use struct config
	map_page(conf->vm_pgtable, DTB_SERIAL, DTB_SERIAL, PTE_U | PTE_R | PTE_W);
}

static void
init_hs(struct vm_config *conf)
{
	uint64 pgtable_pa;

	init_hs_pgtable(conf);
	pgtable_pa = KVA2PA((uint64) conf->vm_pgtable);
	CSRW(hgatp, ATP_MODE_SV39 | pgtable_pa >> 12);
	__asm__ volatile("hfence.gvma");

	CSRW(hstatus, HSTATUS_VSXL | HSTATUS_SPV);
	CSRW(hedeleg, 0ULL);
	CSRW(hideleg, HIDELEG_VSTI | HIDELEG_VSEI);
	CSRW(hvip, 0ULL);
	CSRW(hip, 0ULL);
	CSRW(hie, 0ULL);
	CSRW(hgeie, 0ULL);
	CSRW(hcounteren, HCOUNTEREN_TM);	// vm can now read time
	CSRW(htimedelta, 0ULL);
	CSRW(htval, 0ULL);
	CSRW(htinst, 0ULL);
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
get_config_for_cpu(uint64 hartid)
{
	struct vm_config *conf = 0;
	printf("%p\n", nr_vms);
	for (int i = 0; i < nr_vms; i++) {
		if (config[i].cpu_affinity & (1 << hartid)) {
			if (conf == 0) {
				conf = &config[i];
			} else {
				panic("single CPU assigned to multiple VMs");
			}
		}
	}
	if (conf == 0)
		panic("CPU active, but not assigned to any VM");
	return conf;
}

void __attribute__((noreturn))
vm_run(uint64 hartid)
{
	struct vm_config *conf = get_config_for_cpu(hartid);

	init_vs();
	init_hs(conf);
	init_vcpu(conf);
	(conf->vcpu).x[10] = hartid;	// TODO: this is just testing

	CSRS(sstatus, SSTATUS_SPP);
	CSRW(sepc, conf->entry);
	vm_enter();

	while (1);	// this and noreturn removes function epilogue
}

