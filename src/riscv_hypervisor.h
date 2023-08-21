#ifndef RISCV_HYPERVISOR_H
#define RISCV_HYPERVISOR_H

#include "types.h"
#include "riscv.h"

// Hypervisor extension
// uses S-mode registers for exception, interrupt and address translation subsystems
// offers new HS-mode registers (hsstatus, hideleg, hedeleg)
// offers extra set of VS-mode registers

#define RISCV_HS_MODE (2 << 11)
#define RISCV_VS_MODE (1 << 11)
#define RISCV_VU_MODE (0 << 11)

#define EXC_INSTRUCTION_GUEST_PAGE_FAULT 20
#define EXC_LOAD_GUEST_PAGE_FAULT 21
#define EXC_VIRTUAL_INSTRUCTION 22
#define EXC_STORE_OR_AMO_GUEST_PAGE_FAULT 23


// Hypervisor Status Register

#define HSSTATUS_VSBE (1 << 5)		// Virtual Supervisor Big Endianness
#define HSSTATUS_GVA (1 << 6)		// Guest Virtual Address
#define HSSTATUS_SPV (1 << 7)		// Supervisor Previous Virtualization Mode
#define HSSTATUS_SPVP (1 << 8)		// Supervisor Previous Virtual Privilege
#define HSSTATUS_HU (1 << 9)		// Hypervisor in U-mode
#define HSSTATUS_VGEIN (0b111111 << 12)	// Virtual Guest External Interrupt Number
#define HSSTATUS_VTVM (1 << 20)		// Virtual Trap Virtual Memory
#define HSSTATUS_VTW (1 << 21)		// Virtual Timout Wait
#define HSSTATUS_VTSR (1 << 22)		// Virtual Trap SRET
#define HSSTATUS_VSXL (0b11 << 32)	// effective VSXLEN

static inline uint64
r_hsstatus()
{
	uint64 x;
	asm volatile("csrr %0, hsstatus" : "=r" (x));
	return x;
}

static inline void
w_hsstatus(uint64 x)
{
	asm volatile("csrw hsstatus, %0" : : "r" (x));
}


// Hypervisor Exception Delegation Register

static inline uint64
r_hedeleg()
{
	uint64 x;
	asm volatile("csrr %0, hedeleg" : "=r" (x));
	return x;
}

static inline void
w_hedeleg(uint64 x)
{
	asm volatile("csrw hedeleg, %0" : : "r" (x));
}


// Hypervisor Interrupt Delegation Register

static inline uint64
r_hideleg()
{
	uint64 x;
	asm volatile("csrr %0, hideleg" : "=r" (x));
	return x;
}

static inline void
w_hideleg(uint64 x)
{
	asm volatile("csrw hideleg, %0" : : "r" (x));
}

#endif // RISCV_HYPERVISOR_H
