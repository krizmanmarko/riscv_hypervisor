#include "sbi.h"

static inline struct sbiret
sbi_ecall(long eid, long fid, long a0, long a1, long a2, long a3, long a4,
	  long a5)
{
	register long _a0 asm("a0") = a0;
	register long _a1 asm("a1") = a1;
	register long _a2 asm("a2") = a2;
	register long _a3 asm("a3") = a3;
	register long _a4 asm("a4") = a4;
	register long _a5 asm("a5") = a5;
	register long _a6 asm("a6") = fid;
	register long _a7 asm("a7") = eid;

	__asm__ volatile("ecall"
		: "+r"(_a0), "+r"(_a1)
		: "r"(_a2), "r"(_a3), "r"(_a4), "r"(_a5), "r"(_a6), "r"(_a7)
		: "memory");

	struct sbiret ret = {
		.error = _a0,
		.value = _a1
	};

	return ret;
}

struct sbiret
sbi_get_spec_version(void)
{
	return sbi_ecall(0x10, 0, 0, 0, 0, 0, 0, 0);
}


struct sbiret
sbi_get_impl_id(void)
{
	return sbi_ecall(0x10, 1, 0, 0, 0, 0, 0, 0);
}

struct sbiret 
sbi_get_impl_version(void)
{
	return sbi_ecall(0x10, 2, 0, 0, 0, 0, 0, 0);
}

struct sbiret 
sbi_probe_extension(long extension_id)
{
	return sbi_ecall(0x10, 3, extension_id, 0, 0, 0, 0, 0);
}

struct sbiret 
sbi_get_mvendorid(void)
{
	return sbi_ecall(0x10, 4, 0, 0, 0, 0, 0, 0);
}

struct sbiret 
sbi_get_marchid(void)
{
	return sbi_ecall(0x10, 5, 0, 0, 0, 0, 0, 0);
}

struct sbiret 
sbi_get_mimpid(void)
{
	return sbi_ecall(0x10, 6, 0, 0, 0, 0, 0, 0);
}

// Timer extension (TIME)
struct sbiret 
sbi_set_timer(uint64 stime_value)
{
	return sbi_ecall(0x54494D45, 0, stime_value, 0, 0, 0, 0, 0);
}

// IPI extension (sPI / s-mode IPI)
struct sbiret 
sbi_send_ipi(unsigned long hart_mask, unsigned long hart_mask_base)
{
	return sbi_ecall(0x735049, 0, hart_mask, hart_mask_base, 0, 0, 0, 0);
}


// RFENCE extension (RFNC)
struct sbiret 
sbi_remote_fence_i(unsigned long hart_mask, unsigned long hart_mask_base)
{
	return sbi_ecall(0x52464e43, 0, hart_mask, hart_mask_base, 0, 0, 0, 0);
}

struct sbiret 
sbi_remote_sfence_vma(unsigned long hart_mask,
		      unsigned long hart_mask_base,
		      unsigned long start_addr,
		      unsigned long size)
{
	return sbi_ecall(0x52464e43, 1, hart_mask, hart_mask_base, start_addr,
			 size, 0, 0);
}

struct sbiret
sbi_remote_sfence_vma_asid(unsigned long hart_mask,
			   unsigned long hart_mask_base,
			   unsigned long start_addr,
			   unsigned long size,
			   unsigned long asid)
{
	return sbi_ecall(0x52464e43, 2, hart_mask, hart_mask_base, start_addr,
			 size, asid, 0);
}

struct sbiret
sbi_remote_hfence_gvma_vmid(unsigned long hart_mask,
			    unsigned long hart_mask_base,
			    unsigned long start_addr,
			    unsigned long size,
			    unsigned long vmid)
{
	return sbi_ecall(0x52464e43, 3, hart_mask, hart_mask_base, start_addr,
			 size, vmid, 0);
}

struct sbiret
sbi_remote_hfence_gvma(unsigned long hart_mask,
		       unsigned long hart_mask_base,
		       unsigned long start_addr,
		       unsigned long size)
{
	return sbi_ecall(0x52464e43, 4, hart_mask, hart_mask_base, start_addr,
			 size, 0, 0);
}

struct sbiret
sbi_remote_hfence_vvma_asid(unsigned long hart_mask,
			    unsigned long hart_mask_base,
			    unsigned long start_addr,
			    unsigned long size,
			    unsigned long asid)
{
	return sbi_ecall(0x52464e43, 5, hart_mask, hart_mask_base, start_addr,
			 size, asid, 0);
}

struct sbiret
sbi_remote_hfence_vvma(unsigned long hart_mask,
		       unsigned long hart_mask_base,
		       unsigned long start_addr,
		       unsigned long size)
{
	return sbi_ecall(0x52464e43, 6, hart_mask, hart_mask_base, start_addr,
			 size, 0, 0);
}

// Hart state management extension (HSM)
struct sbiret
sbi_hart_start(unsigned long hartid,
	       unsigned long start_addr,
	       unsigned long opaque)
{
	return sbi_ecall(0x48534d, 0, hartid, start_addr, opaque, 0, 0, 0);
}

struct sbiret
sbi_hart_stop(void)
{
	return sbi_ecall(0x48534d, 1, 0, 0, 0, 0, 0, 0);
}

struct sbiret
sbi_hart_get_status(unsigned long hartid)
{
	return sbi_ecall(0x48534d, 2, hartid, 0, 0, 0, 0, 0);
}

struct sbiret
sbi_hart_suspend(uint32 suspend_type,
		 unsigned long resume_addr,
		 unsigned long opaque)
{
	return sbi_ecall(0x48534d, 3, suspend_type, resume_addr, opaque, 0, 0,
			 0);
}
