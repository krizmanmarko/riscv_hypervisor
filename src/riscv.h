#ifndef RISCV_H
#define RISCV_H

#include "types.h"

// WARL - write any, read legal
// WLRL - write legal, read legal
// WPRI - reserved writes preserve values, reads ignore values (ignored)

#define M_MODE (3 << 11)
#define S_MODE (1 << 11)
#define U_MODE (0 << 11)

#define INT_SUPERVISOR_SW 1
#define INT_MACHINE_SW 3
#define INT_SUPERVISOR_TIM 5
#define INT_MACHINE_TIM 7
#define INT_SUPERVISOR_EXT 9
#define INT_MACHINE_EXT 11

#define EXC_INSTRUCTION_ADDR_MISALIGNED 0
#define EXC_INSTRUCTION_ACCESS_FAULT 1
#define EXC_ILLEGAL_INSTRUCTION 2
#define EXC_BREAKPOINT 3
#define EXC_LOAD_ADDR_MISALIGNED 4
#define EXC_LOAD_ACCESS_FAULT 5
#define EXC_STORE_OR_AMO_ADDRESS_MISALIGNED 6
#define EXC_STORE_OR_AMO_ACCESS_FAULT 7
#define EXC_ECALL_FROM_U 8
#define EXC_ECALL_FROM_S 9
#define EXC_ECALL_FROM_M 11
#define EXC_INSTRUCTION_PAGE_FAULT 12
#define EXC_LOAD_PAGE_FAULT 13
#define EXC_STORE_OR_AMO_PAGE_FAULT 15


// Machine ISA Register (WARL)

#define MISA_EXT_A (1 << 0)	// Atomic extension
#define MISA_EXT_C (1 << 2)	// Compressed extension
#define MISA_EXT_D (1 << 3)	// Double-precision floating-point extension
#define MISA_EXT_F (1 << 5)	// Single-precision floating-point extension
#define MISA_EXT_H (1 << 7)	// Hypervisor extension
#define MISA_EXT_I (1 << 8)	// RV32I/64I/128I base ISA
#define MISA_EXT_M (1 << 12)	// Integer Multiply/Divide extension
#define MISA_EXT_S (1 << 18)	// Supervisor mode implemented
#define MISA_EXT_U (1 << 20)	// User mode implemented

static inline uint64
r_misa()
{
	uint64 x;
	asm volatile("csrr %0, misa" : "=r" (x));
	return x;
}

static inline void
w_misa(uint64 x)
{
	asm volatile("csrw misa, %0" :: "r" (x));
}


// Machine Vendor, Architecture, Implementation and Hart ID Registers

static inline uint32
r_mvendorid()
{
	uint32 x;
	asm volatile("csrr %0, mvendorid" : "=r" (x));
	return x;
}

static inline uint64
r_marchid()
{
	uint64 x;
	asm volatile("csrr %0, marchid" : "=r" (x));
	return x;
}

static inline uint64
r_mimpid()
{
	uint64 x;
	asm volatile("csrr %0, mimpid" : "=r" (x));
	return x;
}

static inline uint64
r_mhartid()
{
	uint64 x;
	asm volatile("csrr %0, mhartid" : "=r" (x));
	return x;
}


// Machine Status Register (WARL)

#define MSTATUS_SIE (1 << 1)		// Supervisor Interrupt Enable
#define MSTATUS_MIE (1 << 3)		// Machine Interrupt Enable
#define MSTATUS_SPIE (1 << 5)		// Supervisor Previous Interrupt Enable
#define MSTATUS_UBE (1 << 6)		// User is big-endian (no effect on fetch)
#define MSTATUS_MPIE (1 << 7)		// Machine Previous Interrupt Enable
#define MSTATUS_SPP (1 << 8)		// Supervisor Previous Privilege
#define MSTATUS_VS (0b11 << 9)		// Vector extension state
#define MSTATUS_MPP (0b11 << 11)	// Machine Previous Privilege
#define MSTATUS_FS (0b11 << 13)		// Floating-point Unit State
#define MSTATUS_XS (0b11 << 15)		// User Mode extensions
#define MSTATUS_MPRV (1 << 17)		// Modify Privilege (set effective privilege)
#define MSTATUS_SUM (1 << 18)		// Permit Supervisor User Memory access
#define MSTATUS_MXR (1 << 19)		// Make Executable Readable
#define MSTATUS_TVM (1 << 20)		// Trap Virtual Memory
#define MSTATUS_TW (1 << 21)		// Timeout Wait
#define MSTATUS_TSR (1 << 22)		// Trap SRET
#define MSTATUS_UXL (0b11 << 32)	// User XLEN
#define MSTATUS_SXL (0b11 << 34)	// Supervisor XLEN
#define MSTATUS_SBE (1 << 36)		// Supervisor is big-endian (no effect on fetch)
#define MSTATUS_MBE (1 << 37)		// Machine is big-endian (no effect on fetch)
#define MSTATUS_SD (1 << 63)		// Something dirty (in FS, VS or XS)

static inline uint64
r_mstatus()
{
	uint64 x;
	asm volatile("csrr %0, mstatus" : "=r" (x));
	return x;
}

static inline void
w_mstatus(uint64 x)
{
	asm volatile("csrw mstatus, %0" :: "r" (x));
}


// Machine Trap-Vector Base-Address Register (WARL)

#define MTVEC_MODE_DIRECT 0
#define MTVEC_MODE_VECTORED 1

static inline uint64
r_mtvec()
{
	uint64 x;
	asm volatile("csrr %0, mtvec" : "=r" (x));
	return x;
}

static inline void
w_mtvec(uint64 x)
{
	asm volatile("csrw mtvec, %0" :: "r" (x));
}


// Machine Interrupt Pending Register

#define MIP_SSIP (1 << INT_SUPERVISOR_SW)
#define MIP_MSIP (1 << INT_MACHINE_SW)
#define MIP_STIP (1 << INT_SUPERVISOR_TIM)
#define MIP_MTIP (1 << INT_MACHINE_TIM)
#define MIP_SEIP (1 << INT_SUPERVISOR_EXT)
#define MIP_MEIP (1 << INT_MACHINE_EXT)

static inline uint64
r_mip()
{
	uint64 x;
	asm volatile("csrr %0, mip" : "=r" (x));
	return x;
}

static inline void
w_mip(uint64 x)
{
	asm volatile("csrw mip, %0" :: "r" (x));
}


// Machine Interrupt Enable Register

#define MIE_SSIE (1 << INT_SUPERVISOR_SW)
#define MIE_MSIE (1 << INT_MACHINE_SW)
#define MIE_STIE (1 << INT_SUPERVISOR_TIM)
#define MIE_MTIE (1 << INT_MACHINE_TIM)
#define MIE_SEIE (1 << INT_SUPERVISOR_EXT)
#define MIE_MEIE (1 << INT_MACHINE_EXT)

static inline uint64
r_mie()
{
	uint64 x;
	asm volatile("csrr %0, mie" : "=r" (x));
	return x;
}

static inline void
w_mie(uint64 x)
{
	asm volatile("csrw mie, %0" :: "r" (x));
}


// Machine Exception Delegation Register

#define MEDELEG_INSTRUCTION_ADDR_MISALIGNED (1 << EXC_INSTRUCTION_ADDR_MISALIGNED)
#define MEDELEG_INSTRUCTION_ACCESS_FAULT (1 << EXC_INSTRUCTION_ACCESS_FAULT)
#define MEDELEG_ILLEGAL_INSTRUCTION (1 << EXC_ILLEGAL_INSTRUCTION)
#define MEDELEG_BREAKPOINT (1 << EXC_BREAKPOINT)
#define MEDELEG_LOAD_ADDR_MISALIGNED (1 << EXC_LOAD_ADDR_MISALIGNED)
#define MEDELEG_LOAD_ACCESS_FAULT (1 << EXC_LOAD_ACCESS_FAULT)
#define MEDELEG_STORE_OR_AMO_ADDRESS_MISALIGNED (1 << EXC_STORE_OR_AMO_ADDRESS_MISALIGNED)
#define MEDELEG_STORE_OR_AMO_ACCESS_FAULT (1 << EXC_STORE_OR_AMO_ACCESS_FAULT)
#define MEDELEG_ECALL_FROM_U (1 << EXC_ECALL_FROM_U)
#define MEDELEG_ECALL_FROM_S (1 << EXC_ECALL_FROM_S)
#define MEDELEG_ECALL_FROM_M (1 << EXC_ECALL_FROM_M)
#define MEDELEG_INSTRUCTION_PAGE_FAULT (1 << EXC_INSTRUCTION_PAGE_FAULT)
#define MEDELEG_LOAD_PAGE_FAULT (1 << EXC_LOAD_PAGE_FAULT)
#define MEDELEG_STORE_OR_AMO_PAGE_FAULT (1 << EXC_STORE_OR_AMO_PAGE_FAULT)

static inline uint64
r_medeleg()
{
	uint64 x;
	asm volatile("csrr %0, medeleg" : "=r" (x));
	return x;
}

static inline void
w_medeleg(uint64 x)
{
	asm volatile("csrw medeleg, %0" :: "r" (x));
}


// Machine Interrupt Delegation Register

#define MIDELEG_SSI (1 << INT_SUPERVISOR_SW)
#define MIDELEG_MSI (1 << INT_MACHINE_SW)
#define MIDELEG_STI (1 << INT_SUPERVISOR_TIM)
#define MIDELEG_MTI (1 << INT_MACHINE_TIM)
#define MIDELEG_SEI (1 << INT_SUPERVISOR_EXT)
#define MIDELEG_MEI (1 << INT_MACHINE_EXT)

static inline uint64
r_mideleg()
{
	uint64 x;
	asm volatile("csrr %0, mideleg" : "=r" (x));
	return x;
}

static inline void
w_mideleg(uint64 x)
{
	asm volatile("csrw mideleg, %0" :: "r" (x));
}


// Machine Counter-Enable Register

#define MCOUNTEREN_CY (1 << 0)		// Cycle
#define MCOUNTEREN_TM (1 << 1)		// Time
#define MCOUNTEREN_IR (1 << 2)		// Instret
#define MCOUNTEREN_HPM3 (1 << 3)	// Hardware Performance Monitor
#define MCOUNTEREN_HPM4 (1 << 4)	// Hardware Performance Monitor
#define MCOUNTEREN_HPM5 (1 << 5)	// Hardware Performance Monitor
#define MCOUNTEREN_HPM6 (1 << 6)	// Hardware Performance Monitor
#define MCOUNTEREN_HPM7 (1 << 7)	// Hardware Performance Monitor
#define MCOUNTEREN_HPM8 (1 << 8)	// Hardware Performance Monitor
#define MCOUNTEREN_HPM9 (1 << 9)	// Hardware Performance Monitor
#define MCOUNTEREN_HPM10 (1 << 10)	// Hardware Performance Monitor
#define MCOUNTEREN_HPM11 (1 << 11)	// Hardware Performance Monitor
#define MCOUNTEREN_HPM12 (1 << 12)	// Hardware Performance Monitor
#define MCOUNTEREN_HPM13 (1 << 13)	// Hardware Performance Monitor
#define MCOUNTEREN_HPM14 (1 << 14)	// Hardware Performance Monitor
#define MCOUNTEREN_HPM15 (1 << 15)	// Hardware Performance Monitor
#define MCOUNTEREN_HPM16 (1 << 16)	// Hardware Performance Monitor
#define MCOUNTEREN_HPM17 (1 << 17)	// Hardware Performance Monitor
#define MCOUNTEREN_HPM18 (1 << 18)	// Hardware Performance Monitor
#define MCOUNTEREN_HPM19 (1 << 19)	// Hardware Performance Monitor
#define MCOUNTEREN_HPM20 (1 << 20)	// Hardware Performance Monitor
#define MCOUNTEREN_HPM21 (1 << 21)	// Hardware Performance Monitor
#define MCOUNTEREN_HPM22 (1 << 22)	// Hardware Performance Monitor
#define MCOUNTEREN_HPM23 (1 << 23)	// Hardware Performance Monitor
#define MCOUNTEREN_HPM24 (1 << 24)	// Hardware Performance Monitor
#define MCOUNTEREN_HPM25 (1 << 25)	// Hardware Performance Monitor
#define MCOUNTEREN_HPM26 (1 << 26)	// Hardware Performance Monitor
#define MCOUNTEREN_HPM27 (1 << 27)	// Hardware Performance Monitor
#define MCOUNTEREN_HPM28 (1 << 28)	// Hardware Performance Monitor
#define MCOUNTEREN_HPM29 (1 << 29)	// Hardware Performance Monitor
#define MCOUNTEREN_HPM30 (1 << 30)	// Hardware Performance Monitor
#define MCOUNTEREN_HPM31 (1 << 31)	// Hardware Performance Monitor

static inline uint32
r_mcounteren()
{
	uint32 x;
	asm volatile("csrr %0, mcounteren" : "=r" (x));
	return x;
}

static inline void
w_mcounteren(uint32 x)
{
	asm volatile("csrw mcounteren, %0" :: "r" (x));
}


// Machine Counter-Inhibit CSR

#define MCOUNTINHIBIT_CY (1 << 0)	// Cycle
#define MCOUNTINHIBIT_IR (1 << 2)	// Instret
#define MCOUNTINHIBIT_HPM3 (1 << 3)	// Hardware Performance Monitor
#define MCOUNTINHIBIT_HPM4 (1 << 4)	// Hardware Performance Monitor
#define MCOUNTINHIBIT_HPM5 (1 << 5)	// Hardware Performance Monitor
#define MCOUNTINHIBIT_HPM6 (1 << 6)	// Hardware Performance Monitor
#define MCOUNTINHIBIT_HPM7 (1 << 7)	// Hardware Performance Monitor
#define MCOUNTINHIBIT_HPM8 (1 << 8)	// Hardware Performance Monitor
#define MCOUNTINHIBIT_HPM9 (1 << 9)	// Hardware Performance Monitor
#define MCOUNTINHIBIT_HPM10 (1 << 10)	// Hardware Performance Monitor
#define MCOUNTINHIBIT_HPM11 (1 << 11)	// Hardware Performance Monitor
#define MCOUNTINHIBIT_HPM12 (1 << 12)	// Hardware Performance Monitor
#define MCOUNTINHIBIT_HPM13 (1 << 13)	// Hardware Performance Monitor
#define MCOUNTINHIBIT_HPM14 (1 << 14)	// Hardware Performance Monitor
#define MCOUNTINHIBIT_HPM15 (1 << 15)	// Hardware Performance Monitor
#define MCOUNTINHIBIT_HPM16 (1 << 16)	// Hardware Performance Monitor
#define MCOUNTINHIBIT_HPM17 (1 << 17)	// Hardware Performance Monitor
#define MCOUNTINHIBIT_HPM18 (1 << 18)	// Hardware Performance Monitor
#define MCOUNTINHIBIT_HPM19 (1 << 19)	// Hardware Performance Monitor
#define MCOUNTINHIBIT_HPM20 (1 << 20)	// Hardware Performance Monitor
#define MCOUNTINHIBIT_HPM21 (1 << 21)	// Hardware Performance Monitor
#define MCOUNTINHIBIT_HPM22 (1 << 22)	// Hardware Performance Monitor
#define MCOUNTINHIBIT_HPM23 (1 << 23)	// Hardware Performance Monitor
#define MCOUNTINHIBIT_HPM24 (1 << 24)	// Hardware Performance Monitor
#define MCOUNTINHIBIT_HPM25 (1 << 25)	// Hardware Performance Monitor
#define MCOUNTINHIBIT_HPM26 (1 << 26)	// Hardware Performance Monitor
#define MCOUNTINHIBIT_HPM27 (1 << 27)	// Hardware Performance Monitor
#define MCOUNTINHIBIT_HPM28 (1 << 28)	// Hardware Performance Monitor
#define MCOUNTINHIBIT_HPM29 (1 << 29)	// Hardware Performance Monitor
#define MCOUNTINHIBIT_HPM30 (1 << 30)	// Hardware Performance Monitor
#define MCOUNTINHIBIT_HPM31 (1 << 31)	// Hardware Performance Monitor

static inline uint32
r_mcountinhibit()
{
	uint32 x;
	asm volatile("csrr %0, mcountinhibit" : "=r" (x));
	return x;
}

static inline void
w_mcountinhibit(uint32 x)
{
	asm volatile("csrw mcountinhibit, %0" :: "r" (x));
}


// Machine Scratch

static inline uint64
r_mscratch()
{
	uint64 x;
	asm volatile("csrr %0, mscratch" : "=r" (x));
	return x;
}

static inline void
w_mscratch(uint64 x)
{
	asm volatile("csrw mscratch, %0" :: "r" (x));
}


// Machine Exception Program Counter

static inline uint64
r_mepc()
{
	uint64 x;
	asm volatile("csrr %0, mepc" : "=r" (x));
	return x;
}

static inline void
w_mepc(uint64 x)
{
	asm volatile("csrw mepc, %0" :: "r" (x));
}


// Machine Cause Register

#define MCAUSE_INT (1 << 63)

static inline uint64
r_mcause()
{
	uint64 x;
	asm volatile("csrr %0, mcause" : "=r" (x));
	return x;
}

static inline void
w_mcause(uint64 x)
{
	asm volatile("csrw mcause, %0" :: "r" (x));
}


// Machine Trap Value Register

static inline uint64
r_mtval()
{
	uint64 x;
	asm volatile("csrr %0, mtval" : "=r" (x));
	return x;
}

static inline void
w_mtval(uint64 x)
{
	asm volatile("csrw mtval, %0" :: "r" (x));
}


// Machine Configuration Pointer Register

/* Not accessible in qemu-virt
static inline uint64
r_mconfigptr()
{
	uint64 x;
	asm volatile("csrr %0, mconfigptr" : "=r" (x));
	return x;
}
*/


// Machine Environment Configuration Register

#define MENVCFG_FIOM (1 << 0)		// Fence of I/O Implies Memory

static inline uint64
r_menvcfg()
{
	uint64 x;
	asm volatile("csrr %0, menvcfg" : "=r" (x));
	return x;
}

static inline void
w_menvcfg(uint64 x)
{
	asm volatile("csrw menvcfg, %0" :: "r" (x));
}


// Physical Memory Protection - PMP

static inline void
w_pmpcfg0(uint64 x)
{
	asm volatile("csrw pmpcfg0, %0" : : "r" (x));
}

static inline void
w_pmpaddr0(uint64 x)
{
	asm volatile("csrw pmpaddr0, %0" : : "r" (x));
}


// ... TODO
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//

static inline uint64
r_satp()
{
	uint64 x;
	asm volatile("csrr %0, satp" : "=r" (x));
	return x;
}

static inline void
w_satp(uint64 x)
{
	asm volatile("csrw satp, %0" : : "r" (x));
}

static inline uint64
r_stvec()
{
	uint64 x;
	asm volatile("csrr %0, stvec" : "=r" (x));
	return x;
}

static inline void
w_stvec(uint64 x)
{
	asm volatile("csrw stvec, %0" : : "r" (x));
}

#endif // RISCV_H
