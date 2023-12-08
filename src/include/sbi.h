#ifndef SBI_H
#define SBI_H

#include "types.h"

#define SBI_SUCCESS (0)
#define SBI_ERR_FAILED (-1)
#define SBI_ERR_NOT_SUPPORTED (-2)
#define SBI_ERR_INVALID_PARAM (-3)
#define SBI_ERR_DENIED (-4)
#define SBI_ERR_INVALID_ADDRESS (-5)
#define SBI_ERR_ALREADY_AVAILABLE (-6)
#define SBI_ERR_ALREADY_STARTED (-7)
#define SBI_ERR_ALREADY_STOPPED (-8)

#ifndef __ASSEMBLER__

struct sbiret {
	long error;	// a0
	long value;	// a1
};

#endif // __ASSEMBLER__

// unsigned long hart_mask: is a scalar bit-vector containing hartids
// unsigned long hart_mask_base: is the starting hartid from which bit-vector must be computed.

#endif // SBI_H

// Base extension (EID=0x10) - retval in a0, a1
struct sbiret sbi_get_spec_version(void);
struct sbiret sbi_get_impl_id(void);
struct sbiret sbi_get_impl_version(void);
struct sbiret sbi_probe_extension(long extension_id);
struct sbiret sbi_get_mvendorid(void);
struct sbiret sbi_get_marchid(void);
struct sbiret sbi_get_mimpid(void);

// Timer extension (TIME)
struct sbiret sbi_set_timer(uint64 stime_value);

// IPI extension (sPI / s-mode IPI)
struct sbiret sbi_send_ipi(
	unsigned long hart_mask,
	unsigned long hart_mask_base
);

// RFENCE extension (RFNC)
struct sbiret sbi_remote_fence_i(
	unsigned long hart_mask,
	unsigned long hart_mask_base
);
struct sbiret sbi_remote_sfence_vma(
	unsigned long hart_mask,
	unsigned long hart_mask_base,
	unsigned long start_addr,
	unsigned long size
);
struct sbiret sbi_remote_sfence_vma_asid(
	unsigned long hart_mask,
	unsigned long hart_mask_base,
	unsigned long start_addr,
	unsigned long size,
	unsigned long asid
);
struct sbiret sbi_remote_hfence_gvma_vmid(
	unsigned long hart_mask,
	unsigned long hart_mask_base,
	unsigned long start_addr,
	unsigned long size,
	unsigned long vmid
);
struct sbiret sbi_remote_hfence_gvma(
	unsigned long hart_mask,
	unsigned long hart_mask_base,
	unsigned long start_addr,
	unsigned long size
);
struct sbiret sbi_remote_hfence_vvma_asid(
	unsigned long hart_mask,
	unsigned long hart_mask_base,
	unsigned long start_addr,
	unsigned long size,
	unsigned long asid
);
struct sbiret sbi_remote_hfence_vvma(
	unsigned long hart_mask,
	unsigned long hart_mask_base,
	unsigned long start_addr,
	unsigned long size
);

// Hart state management extension (HSM)
struct sbiret sbi_hart_start(
	unsigned long hartid,
	unsigned long start_addr,
	unsigned long opaque
);
struct sbiret sbi_hart_stop(void);
struct sbiret sbi_hart_get_status(unsigned long hartid);
struct sbiret sbi_hart_suspend(
	uint32 suspend_type,
	unsigned long resume_addr,
	unsigned long opaque
);
