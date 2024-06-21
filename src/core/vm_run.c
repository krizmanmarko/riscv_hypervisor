#include "bits.h"
#include "defs.h"
#include "dtb.h"
#include "lock.h"
#include "memory.h"
#include "riscv.h"
#include "stdio.h"
#include "string.h"
#include "types.h"
#include "vcpu.h"
#include "vm_config.h"

static void init_hs_pgtable();
static void init_hs();
static void init_vs();
void __attribute__((noreturn)) vm_run();

static struct barrier bar = BARRIER_INITIALIZER(DTB_NR_CPUS);

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

	// TODO use struct config
	map_page(conf->vm_pgtable, DTB_SERIAL, DTB_SERIAL, PTE_U | PTE_R | PTE_W);
	map_pages(conf->vm_pgtable, DTB_PLIC, DTB_PLIC, 0x600000, PTE_U | PTE_R | PTE_W);
}

static void
init_hs(struct vm_config *conf)
{
	uint64 pgtable_pa;

	pgtable_pa = KVA2PA((uint64) conf->vm_pgtable);
	CSRW(hgatp, ATP_MODE_SV39 | pgtable_pa >> 12);
	__asm__ volatile("hfence.gvma");

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

#define PRINTCSR(reg) printf(#reg "	: %p\n", CSRR(reg));

	// testing external interrupts
	PRINTCSR(hideleg)
	PRINTCSR(sstatus)
	CSRS(sie, -1);
	PRINTCSR(sie)
	CSRS(hie, -1);
	PRINTCSR(hie)
	CSRS(hideleg, -1);
	PRINTCSR(hideleg)
	CSRS(hstatus, 1 << 12);		// set smallest VGEIN possible
	PRINTCSR(hstatus)
	CSRS(hgeie, -1);
	PRINTCSR(hgeie)
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
	for (int i = 0; i < nr_vms; i++)
		if (config[i].cpu_affinity & (1 << hartid))
			return &config[i];
	panic("No way (CPU configuration was already checked)");
}

static void
init_vm_barriers()
{
	for (int i = 0; i < nr_vms; i++) {
		init_barrier(
			&config[i].bar,
			count_set_bits(config[i].cpu_affinity)
		);
	}
}

void
check_cpu_configuration()
{
	int nr_cpus = 0;

	for (int i = 0; i < nr_vms; i++)
		for (int j = 0; j < i; j++)
			if (config[i].cpu_affinity & config[j].cpu_affinity)
				panic("some VMs share the same CPU (%d, %d)", j, i);

	for (int i = 0; i < nr_vms; i++)
		nr_cpus += count_set_bits(config[i].cpu_affinity);

	if (nr_cpus != DTB_NR_CPUS)
		panic("number of CPUs not configured correctly\nvm_config: %d, dtb: %d", nr_cpus, DTB_NR_CPUS);
}

void __attribute__((noreturn))
vm_run(uint64 hartid)
{
	uint64 vhartid;
	struct vm_config *conf;

	if (hartid == 0) {
		init_vm_barriers();
		check_cpu_configuration();
	}
	wait_barrier(&bar);

	conf = get_config_for_cpu(hartid);
	vhartid = init_vcpu(conf);
	if (vhartid == 0)
		init_hs_pgtable(conf);
	wait_barrier(&conf->bar);

	init_hs(conf);
	init_vs();

	CSRS(sstatus, SSTATUS_SPP);
	CSRW(sepc, conf->entry);
	vm_enter();

	while (1);	// this and noreturn removes function epilogue
}
