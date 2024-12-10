#ifndef RISCV_H
#define RISCV_H // target: riscv64 (qemu-virt)

// Nominal privilege level
#define M_MODE 3
#define S_MODE 1
#define U_MODE 0

#define XLEN_128 (3UL)
#define XLEN_64 (2UL)
#define XLEN_32 (1UL)

#define STATUS_SIE (1UL << 1)		// Supervisor Interrupt Enable
#define STATUS_SPIE (1UL << 5)		// Supervisor Previous Interrupt Enable
#define STATUS_UBE (1UL << 6)		// User is big-endian (no effect on fetch)
#define STATUS_SPP (1UL << 8)		// Supervisor Previous Privilege
#define STATUS_VS (0b11UL << 9)		// Vector extension state
#define STATUS_FS (0b11UL << 13)	// Floating-point Unit State
#define STATUS_XS (0b11UL << 15)	// User Mode extensions
#define STATUS_SUM (1UL << 18)		// Permit Supervisor User Memory access
#define STATUS_MXR (1UL << 19)		// Make Executable Readable
#define STATUS_SD (1UL << (__riscv_xlen - 1))		// Something dirty (in FS, VS or XS)

#if __riscv_xlen == 64
#define STATUS_UXL (0b11UL << 32)	// User XLEN
#define STATUS_UXL_64 (XLEN_64 << 32)
#endif

#define TVEC_MODE (0b11)
#define TVEC_MODE_DIRECT 0
#define TVEC_MODE_VECTORED 1

#define INT_SUPERVISOR_SW 1
#define INT_VIRTUAL_SUPERVISOR_SW 2
#define INT_MACHINE_SW 3
#define INT_SUPERVISOR_TIM 5
#define INT_VIRTUAL_SUPERVISOR_TIM 6
#define INT_MACHINE_TIM 7
#define INT_SUPERVISOR_EXT 9
#define INT_VIRTUAL_SUPERVISOR_EXT 10
#define INT_MACHINE_EXT 11
#define INT_SUPERVISOR_GUEST_EXT 12	// check out section on hip, hie, hvip

#define INT_SSI (1UL << INT_SUPERVISOR_SW)
#define INT_VSSI (1UL << INT_VIRTUAL_SUPERVISOR_SW)
#define INT_MSI (1UL << INT_MACHINE_SW)
#define INT_STI (1UL << INT_SUPERVISOR_TIM)
#define INT_VSTI (1UL << INT_VIRTUAL_SUPERVISOR_TIM)
#define INT_MTI (1UL << INT_MACHINE_TIM)
#define INT_SEI (1UL << INT_SUPERVISOR_EXT)
#define INT_VSEI (1UL << INT_VIRTUAL_SUPERVISOR_EXT)
#define INT_MEI (1UL << INT_MACHINE_EXT)
#define INT_SGEI (1UL << INT_SUPERVISOR_GUEST_EXT)	// check out hypervisor mip

#define COUNT_CY (1UL << 0)		// Cycle
#define COUNT_TM (1UL << 1)		// Time
#define COUNT_IR (1UL << 2)		// Instret
#define COUNT_HPM(n) (1UL << (n))	// Hardware Performance Monitor [3-31]

#define EXC_INSTRUCTION_ADDR_MISALIGNED 0
#define EXC_INSTRUCTION_ACCESS_FAULT 1
#define EXC_ILLEGAL_INSTRUCTION 2
#define EXC_BREAKPOINT 3
#define EXC_LOAD_ADDR_MISALIGNED 4
#define EXC_LOAD_ACCESS_FAULT 5
#define EXC_STORE_OR_AMO_ADDRESS_MISALIGNED 6
#define EXC_STORE_OR_AMO_ACCESS_FAULT 7
#define EXC_ECALL_FROM_U 8
#define EXC_ECALL_FROM_HS 9	// S_MODE
#define EXC_ECALL_FROM_VS 10
#define EXC_ECALL_FROM_M 11
#define EXC_INSTRUCTION_PAGE_FAULT 12
#define EXC_LOAD_PAGE_FAULT 13
#define EXC_STORE_OR_AMO_PAGE_FAULT 15
#define EXC_INSTRUCTION_GUEST_PAGE_FAULT 20
#define EXC_LOAD_GUEST_PAGE_FAULT 21
#define EXC_VIRTUAL_INSTRUCTION 22
#define EXC_STORE_OR_AMO_GUEST_PAGE_FAULT 23

#define CAUSE_INT (1UL << (__riscv_xlen - 1))


// TODO: is this needed? (possibly for H check)
#define MISA_EXT_A (1UL << 0)	// Atomic extension
#define MISA_EXT_C (1UL << 2)	// Compressed extension
#define MISA_EXT_D (1UL << 3)	// Double-precision floating-point extension
#define MISA_EXT_F (1UL << 5)	// Single-precision floating-point extension
#define MISA_EXT_H (1UL << 7)	// Hypervisor extension
#define MISA_EXT_I (1UL << 8)	// RV32I/64I/128I base ISA
#define MISA_EXT_M (1UL << 12)	// Integer Multiply/Divide extension
#define MISA_EXT_S (1UL << 18)	// Supervisor mode implemented
#define MISA_EXT_U (1UL << 20)	// User mode implemented


// Supervisor Status Register
// Supervisor Trap Vector Base Address Register
// Supervisor Interrupt Pending Register
// Supervisor Interrupt Enable Register
// Supervisor Counter-Enable Register
// Supervisor Scratch Register
// Supervisor Exception Program Counter
// Supervisor Cause Register
// Supervisor Trap Value Register
// Supervisor Environment Configuration Register
// Supervisor Address Translation and Protection Register


// Hypervisor Status Register
// Hypervisor Exception Delegation Register
// Hypervisor Interrupt Delegation Register
// Hypervisor Virtual Interrupt Pending Register
// Hypervisor Interrupt Pending Register
// Hypervisor Interrupt Enable Register
// Hypervisor Guest External Interrupt Pending Register
// Hypervisor Guest External Interrupt Enable Register
// Hypervisor Environment Configuration Register
// Hypervisor Counter-Enable Register
// Hypervisor Time Delta Register
// Hypervisor Trap Value Register

// Hypervisor Trap Instruction Register
#define HTINST_READ 0x3000	// TODO: is this in RV32 as well
#define HTINST_WRITE 0x3020

// Hypervisor Guest Address Translation and Protection Register


// Virtual Supervisor Status Register
// Virtual Supervisor Trap Vector Base Address Register
// Virtual Supervisor Interrupt Pending Register
// Virtual Supervisor Interrupt Enable Register
// Virtual Supervisor Scratch Register
// Virtual Supervisor Exception Program Counter Register
// Virtual Supervisor Cause Register
// Virtual Supervisor Trap Value Register
// Virtual Supervisor Address Translation and Protection Register

#ifndef __ASSEMBLER__

// r -> register
// K -> https://gcc.gnu.org/onlinedocs/gcc/Machine-Constraints.html under risc-v
#define CSRR(csr) ({ \
	unsigned long _tmp; \
	__asm__ volatile("csrr  %0, " #csr ";" \
		: "=r" (_tmp) \
		: \
		: "memory" \
	); \
	_tmp; \
})

#define CSRW(csr, val) \
	__asm__ volatile("csrw  " #csr ", %0;" : : "rK" (val) : "memory")

#define CSRC(csr, mask) \
	__asm__ volatile("csrc " #csr ", %0;" : : "rK" (mask) : "memory")

#define CSRS(csr, mask) \
	__asm__ volatile("csrs " #csr ", %0;" : : "rK" (mask) : "memory")

// width is one of: b, bu, h, hu, w, wu, d
#define HLV(width, addr) ({\
	unsigned long _tmp; \
	__asm__ volatile("hlv." #width " %1, 0(%0);" \
		: "=r" (_tmp) \
		: "rK" (addr) \
		: "memory" \
	); \
	_tmp; \
})

#define HSV(width, val, addr) \
	__asm__ volatile("hsv." #width " %0, 0(%1);" \
		: \
		: "r" (val), "r" (addr) \
		: "memory" \
	);

//#define HLVX()

#endif // __ASSEMBLER__
#endif // RISCV_H
