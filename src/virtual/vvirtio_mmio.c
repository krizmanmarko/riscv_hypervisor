#include "bits.h"
#include "defs.h"
#include "emulate.h"
#include "memory.h"
#include "riscv.h"
#include "stdio.h"
#include "types.h"
#include "vcpu.h"

#define VIRTIO_MMIO_QUEUE_DESC_LOW 0x80
#define VIRTIO_MMIO_QUEUE_DESC_HIGH 0x84
#define VIRTIO_MMIO_DRIVER_DESC_LOW 0x90
#define VIRTIO_MMIO_DRIVER_DESC_HIGH 0x94
#define VIRTIO_MMIO_DEVICE_DESC_LOW 0xa0
#define VIRTIO_MMIO_DEVICE_DESC_HIGH 0xa4

/* virtio emulation is done to set these correctly (xv6-riscv, virtio_disk.c)
  // write physical addresses.
  *R(VIRTIO_MMIO_QUEUE_DESC_LOW) = (uint64)disk.desc;
  *R(VIRTIO_MMIO_QUEUE_DESC_HIGH) = (uint64)disk.desc >> 32;
  *R(VIRTIO_MMIO_DRIVER_DESC_LOW) = (uint64)disk.avail;
  *R(VIRTIO_MMIO_DRIVER_DESC_HIGH) = (uint64)disk.avail >> 32;
  *R(VIRTIO_MMIO_DEVICE_DESC_LOW) = (uint64)disk.used;
  *R(VIRTIO_MMIO_DEVICE_DESC_HIGH) = (uint64)disk.used >> 32;
*/

// holds 32 bit value, extra bit is necessary to tell if it was initialized
static int64 queue_desc_low = -1;
static int64 queue_desc_high = -1;
static int64 driver_desc_low = -1;
static int64 driver_desc_high = -1;
static int64 device_desc_low = -1;
static int64 device_desc_high = -1;

static void
parse_htinst(struct instr *instr)
{
	uint64 htinst = CSRR(htinst);

	instr->opcode = get_value(htinst, 0, 7);
	instr->rd = get_value(htinst, 7, 5);
	instr->funct3 = get_value(htinst, 12, 3);
	instr->addr_offset = get_value(htinst, 15, 5);
	instr->rs2 = get_value(htinst, 20, 5);

	if (instr->addr_offset != 0)
		panic("Misaligned access with C extension???");
	if (get_value(htinst, 25, 7) != 0)
		panic("VIRTIO does not support this instruction (AMO?)");
}

static void
write_queue_desc(pte_t *pgtable)
{
	if (queue_desc_high < 0 || queue_desc_low < 0)
		return;
	uint64 gpa = queue_desc_high << 32 | queue_desc_low;
	uint64 pa = convert_va_to_pa(pgtable, gpa, 1);
	uint32 low = (uint32) pa;
	uint32 high = (uint32) (pa >> 32);
	volatile uint32 *addr = (uint32 *)(DTB_VIRTIO0 + VIRTIO_MMIO_QUEUE_DESC_LOW);
	*addr++ = low;
	*addr = high;
	printf("written physical address %p\n", pa);
}

static void
write_driver_desc(pte_t *pgtable)
{
	if (driver_desc_high < 0 || driver_desc_low < 0)
		return;
	uint64 gpa = driver_desc_high << 32 | driver_desc_low;
	uint64 pa = convert_va_to_pa(pgtable, gpa, 1);
	uint32 low = (uint32) pa;
	uint32 high = (uint32) (pa >> 32);
	volatile uint32 *addr = (uint32 *)(DTB_VIRTIO0 + VIRTIO_MMIO_DRIVER_DESC_LOW);
	*addr++ = low;
	*addr = high;
	printf("written physical address %p\n", pa);
}

static void
write_device_desc(pte_t *pgtable)
{
	if (device_desc_high < 0 || device_desc_low < 0)
		return;
	uint64 gpa = device_desc_high << 32 | device_desc_low;
	uint64 pa = convert_va_to_pa(pgtable, gpa, 1);
	uint32 low = (uint32) pa;
	uint32 high = (uint32) (pa >> 32);
	volatile uint32 *addr = (uint32 *)(DTB_VIRTIO0 + VIRTIO_MMIO_DEVICE_DESC_LOW);
	*addr++ = low;
	*addr = high;
	printf("written physical address %p\n", pa);
}

// only responds to first write to desc low/high
// both writes must happen to actually write the value (convertng GPA -> PA)
// xv6-riscv does this, but other guests might not behave so nicely
static void
emulate_store(struct instr *instr)
{
	struct vcpu *vcpu;
	uint64 offset;
	uint32 val;

	vcpu = get_vcpu();
	offset = instr->addr % PAGE_SIZE;
	val = vcpu->regs.x[instr->rs2];
	switch (offset) {
	case VIRTIO_MMIO_QUEUE_DESC_LOW:
		if (queue_desc_low < 0) {
			queue_desc_low = val;
			write_queue_desc(vcpu->conf->vm_pgtable);
		} else {
			panic("virtio queue descriptor (low) can only be configured once");
		}
		break;
	case VIRTIO_MMIO_QUEUE_DESC_HIGH:
		if (queue_desc_high < 0) {
			queue_desc_high = val;
			write_queue_desc(vcpu->conf->vm_pgtable);
		} else {
			panic("virtio queue descriptor (high) can only be configured once");
		}
		break;
	case VIRTIO_MMIO_DRIVER_DESC_LOW:
		if (driver_desc_low < 0) {
			driver_desc_low = val;
			write_driver_desc(vcpu->conf->vm_pgtable);
		} else {
			panic("virtio driver descriptor (low) can only be configured once");
		}
		break;
	case VIRTIO_MMIO_DRIVER_DESC_HIGH:
		if (driver_desc_high < 0) {
			driver_desc_high = val;
			write_driver_desc(vcpu->conf->vm_pgtable);
		} else {
			panic("virtio driver descriptor (high) can only be configured once");
		}
		break;
	case VIRTIO_MMIO_DEVICE_DESC_LOW:
		if (device_desc_low < 0) {
			device_desc_low = val;
			write_device_desc(vcpu->conf->vm_pgtable);
		} else {
			panic("virtio device descriptor (low) can only be configured once");
		}
		break;
	case VIRTIO_MMIO_DEVICE_DESC_HIGH:
		if (device_desc_high < 0) {
			device_desc_high = val;
			write_device_desc(vcpu->conf->vm_pgtable);
		} else {
			panic("virtio device descriptor (high) can only be configured once");
		}
		break;
	default:
		*((uint32 *)instr->addr) = val;
		break;
	}

	// no need to edit VM's regs

	if (instr->opcode & 2)
		CSRW(sepc, CSRR(sepc) + 4);	// uncompressed store
	else
		CSRW(sepc, CSRR(sepc) + 2);	// compressed store
}

void
virtio_handle_store_or_amo_page_fault(uint64 addr)
{
	struct instr instr;
	parse_htinst(&instr);

	if ((instr.opcode | 2) != 0x23)	// replacing bit 1 with 1 (isa 8.6.3)
		panic("Bad opcode for store");
	if (instr.funct3 != 2)
		panic("Bad funct3 for store");

	instr.addr = addr;
	emulate_store(&instr);
}
