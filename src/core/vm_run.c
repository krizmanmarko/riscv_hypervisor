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
	int i, rv;
	struct mmio_dev *dev;

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

	i = 0;
	while ((dev = conf->devices[i++]) != 0) {	// dev != 0
		rv = map_pages(
			conf->vm_pgtable,
			dev->base_virt,
			dev->base_phys,
			dev->size,
			dev->perm | PTE_U
		);
		if (rv < 0) {
			panic("Failed to map MMIO device %d", i);
		}
	}
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
	CSRW(hie, HIE_VSTIE);
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

	// check CPU affinity overlap
	for (int i = 0; i < nr_vms; i++)
		for (int j = 0; j < i; j++)
			if (config[i].cpu_affinity & config[j].cpu_affinity)
				panic("VMs share the same CPU (%d, %d)", j, i);

	// check number of CPUs assigned
	for (int i = 0; i < nr_vms; i++)
		nr_cpus += count_set_bits(config[i].cpu_affinity);
	if (nr_cpus != DTB_NR_CPUS)
		panic("CPU number misconfiguration\nvm_config: %d, dtb: %d",
			nr_cpus, DTB_NR_CPUS);
}

void __attribute__((noreturn))
vm_run(uint64 hartid)
{
	struct vcpu *vcpu;

	if (hartid == 0) {
		init_vm_barriers();
		check_cpu_configuration();
	}
	wait_barrier(&bar);

	vcpu = init_vcpu();
	if (vcpu->vhartid == 0)
		init_hs_pgtable(vcpu->conf);
	wait_barrier(&vcpu->conf->bar);

	init_vs();
	init_hs(vcpu->conf);

	CSRS(sstatus, SSTATUS_SPP);
	CSRW(sepc, vcpu->conf->entry);
	vm_enter();

	while (1);	// this combined with noreturn removes function epilogue
}
