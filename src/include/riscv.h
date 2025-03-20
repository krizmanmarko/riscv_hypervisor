#ifndef RISCV_H
#define RISCV_H // target: riscv64 (qemu-virt)

// Nominal privilege level
#define M_MODE 3
#define S_MODE 1
#define U_MODE 0

#define XLEN_64 (2UL)

// All mode common register fields
#define STATUS_SIE (1UL << 1)		// Supervisor Interrupt Enable
#define STATUS_MIE (1UL << 3)		// Machine Interrupt Enable
#define STATUS_SPIE (1UL << 5)		// Supervisor Previous Interrupt Enable
#define STATUS_UBE (1UL << 6)		// User is big-endian (no effect on fetch)
#define STATUS_MPIE (1UL << 7)		// Machine Previous Interrupt Enable
#define STATUS_SPP (1UL << 8)		// Supervisor Previous Privilege
#define STATUS_VS (0b11UL << 9)		// Vector extension state
#define STATUS_MPP (0b11UL << 11)	// Machine Previous Privilege
#define STATUS_MPP_M (M_MODE << 11)
#define STATUS_MPP_S (S_MODE << 11)
#define STATUS_MPP_U (U_MODE << 11)
#define STATUS_FS (0b11UL << 13)	// Floating-point Unit State
#define STATUS_XS (0b11UL << 15)	// User Mode extensions
#define STATUS_MPRV (1UL << 17)		// Modify Privilege (use previous privilege privileges for loads and stores)
#define STATUS_SUM (1UL << 18)		// Permit Supervisor User Memory access
#define STATUS_MXR (1UL << 19)		// Make Executable Readable
#define STATUS_TVM (1UL << 20)		// Trap Virtual Memory
#define STATUS_TW (1UL << 21)		// Timeout Wait
#define STATUS_TSR (1UL << 22)		// Trap SRET
#define STATUS_UXL (0b11UL << 32)	// User XLEN
#define STATUS_UXL_64 (XLEN_64 << 32)
#define STATUS_SXL (0b11UL << 34)	// Supervisor XLEN
#define STATUS_SXL_64 (XLEN_64 << 34)
#define STATUS_SBE (1UL << 36)		// Supervisor is big-endian (no effect on fetch)
#define STATUS_MBE (1UL << 37)		// Machine is big-endian (no effect on fetch)
#define STATUS_GVA (1UL << 38)		// Guest Virtual Address
#define STATUS_SD (1UL << 63)		// Something dirty (in FS, VS or XS)

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
//#define EXC_ECALL_FROM_S 9
#define EXC_ECALL_FROM_HS 9
#define EXC_ECALL_FROM_VS 10
#define EXC_ECALL_FROM_M 11
#define EXC_INSTRUCTION_PAGE_FAULT 12
#define EXC_LOAD_PAGE_FAULT 13
#define EXC_STORE_OR_AMO_PAGE_FAULT 15
#define EXC_INSTRUCTION_GUEST_PAGE_FAULT 20
#define EXC_LOAD_GUEST_PAGE_FAULT 21
#define EXC_VIRTUAL_INSTRUCTION 22
#define EXC_STORE_OR_AMO_GUEST_PAGE_FAULT 23

#define CAUSE_INT (1UL << 63)

#define ENVCFG_FIOM (1UL << 0)		// Fence of I/O Implies Memory

#define ATP_PPN (0xfffffffffffUL)	// 44-bit physical page number
#define ATP_ASID (0xffffUL << 44)	// Virtual Machine Identifier
#define ATP_ASID_N(n) ((n) << 44)
#define ATP_MODE (0b1111UL << 60)
#define ATP_MODE_BARE (0b0000UL << 60)
#define ATP_MODE_SV39 (0b1000UL << 60)

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
#define SSTATUS_SIE STATUS_SIE
#define SSTATUS_SPIE STATUS_SPIE
#define SSTATUS_UBE STATUS_UBE
#define SSTATUS_SPP STATUS_SPP
#define SSTATUS_VS STATUS_VS
#define SSTATUS_FS STATUS_FS
#define SSTATUS_XS STATUS_XS
#define SSTATUS_SUM STATUS_SUM
#define SSTATUS_MXR STATUS_MXR
#define SSTATUS_UXL STATUS_UXL
#define SSTATUS_SD STATUS_SD

// Supervisor Trap Vector Base Address Register

// Supervisor Interrupt Pending Register
#define SIP_SSIP INT_SSI
#define SIP_STIP INT_STI
#define SIP_SEIP INT_SEI

// Supervisor Interrupt Enable Register
#define SIE_SSIE INT_SSI
#define SIE_STIE INT_STI
#define SIE_SEIE INT_SEI

// Supervisor Counter-Enable Register
#define SCOUNTEREN_CY COUNT_CY
#define SCOUNTEREN_TM COUNT_TM
#define SCOUNTEREN_IR COUNT_IR
#define SCOUNTEREN_HPM(n) (COUNT_HPM(n))

// Supervisor Scratch Register
// Supervisor Exception Program Counter
// Supervisor Cause Register
// Supervisor Trap Value Register

// Supervisor Environment Configuration Register
#define SENVCFG_FIOM ENVCFG_FIOM

// Supervisor Address Translation and Protection Register


// Hypervisor Status Register
#define HSTATUS_VSBE (1UL << 5)		// Virtual Supervisor Big Endianness
#define HSTATUS_GVA STATUS_GVA		// Guest Virtual Address
#define HSTATUS_SPV (1UL << 7)		// Supervisor Previous Virtualization Mode
#define HSTATUS_SPVP (1UL << 8)		// Supervisor Previous Virtual Privilege
#define HSTATUS_HU (1UL << 9)		// Hypervisor in U-mode
#define HSTATUS_VGEIN (0x3fUL << 12)	// Virtual Guest External Interrupt Number
#define HSTATUS_VTVM (1UL << 20)	// Virtual Trap Virtual Memory
#define HSTATUS_VTW (1UL << 21)		// Virtual Timeout Wait
#define HSTATUS_VTSR (1UL << 22)	// Virtual Trap SRET
#define HSTATUS_VSXL (0b11UL << 32)	// effective VSXLEN
#define HSTATUS_VSXL_64 (XLEN_64 << 32)

// Hypervisor Exception Delegation Register
#define HEDELEG_INSTRUCTION_ADDR_MISALIGNED (1UL << EXC_INSTRUCTION_ADDR_MISALIGNED)
#define HEDELEG_INSTRUCTION_ACCESS_FAULT (1UL << EXC_INSTRUCTION_ACCESS_FAULT)
#define HEDELEG_ILLEGAL_INSTRUCTION (1UL << EXC_ILLEGAL_INSTRUCTION)
#define HEDELEG_BREAKPOINT (1UL << EXC_BREAKPOINT)
#define HEDELEG_LOAD_ADDR_MISALIGNED (1UL << EXC_LOAD_ADDR_MISALIGNED)
#define HEDELEG_LOAD_ACCESS_FAULT (1UL << EXC_LOAD_ACCESS_FAULT)
#define HEDELEG_STORE_OR_AMO_ADDRESS_MISALIGNED (1UL << EXC_STORE_OR_AMO_ADDRESS_MISALIGNED)
#define HEDELEG_STORE_OR_AMO_ACCESS_FAULT (1UL << EXC_STORE_OR_AMO_ACCESS_FAULT)
#define HEDELEG_ECALL_FROM_U (1UL << EXC_ECALL_FROM_U)
//#define HEDELEG_ECALL_FROM_HS (1UL << EXC_ECALL_FROM_HS)	// read-only 0
//#define HEDELEG_ECALL_FROM_VS (1UL << EXC_ECALL_FROM_VS)	// read-only 0
//#define HEDELEG_ECALL_FROM_M (1UL << EXC_ECALL_FROM_M)	// read-only 0
#define HEDELEG_INSTRUCTION_PAGE_FAULT (1UL << EXC_INSTRUCTION_PAGE_FAULT)
#define HEDELEG_LOAD_PAGE_FAULT (1UL << EXC_LOAD_PAGE_FAULT)
#define HEDELEG_STORE_OR_AMO_PAGE_FAULT (1UL << EXC_STORE_OR_AMO_PAGE_FAULT)
//#define HEDELEG_INSTRUCTION_GUEST_PAGE_FAULT (1UL << EXC_INSTRUCTION_GUEST_PAGE_FAULT)	// read-only 0
//#define HEDELEG_LOAD_GUEST_PAGE_FAULT (1UL << EXC_LOAD_GUEST_PAGE_FAULT)			// read-only 0
//#define HEDELEG_VIRTUAL_INSTRUCTION (1UL << EXC_VIRTUAL_INSTRUCTION)				// read-only 0
//#define HEDELEG_STORE_OR_AMO_GUEST_PAGE_FAULT (1UL << EXC_STORE_OR_AMO_GUEST_PAGE_FAULT)	// read-only 0

// Hypervisor Interrupt Delegation Register
#define HIDELEG_VSSI INT_VSSI
#define HIDELEG_VSTI INT_VSTI
#define HIDELEG_VSEI INT_VSEI

// Hypervisor Virtual Interrupt Pending Register
#define HVIP_VSSIP INT_VSSI
#define HVIP_VSTIP INT_VSTI
#define HVIP_VSEIP INT_VSEI

// Hypervisor Interrupt Pending Register
#define HIP_VSSIP INT_VSSI
#define HIP_VSTIP INT_VSTI
#define HIP_VSEIP INT_VSEI
#define HIP_SGEIP INT_SGEI

// Hypervisor Interrupt Enable Register
#define HIE_VSSIE INT_VSSI
#define HIE_VSTIE INT_VSTI
#define HIE_VSEIE INT_VSEI
#define HIE_SGEIE INT_SGEI	// TODO: is this correct? should it be SIE_...

// Hypervisor Guest External Interrupt Pending Register
// Hypervisor Guest External Interrupt Enable Register

// Hypervisor Environment Configuration Register
#define HENVCFG_FIOM ENVCFG_FIOM
#define HENVCFG_SSTC (1ULL << 63)
#define HENVCFG_PBMTE (1ULL << 62)
#define HENVCFG_ADUE (1ULL << 61)

// Hypervisor Counter-Enable Register
#define HCOUNTEREN_CY COUNT_CY
#define HCOUNTEREN_TM COUNT_TM
#define HCOUNTEREN_IR COUNT_IR
#define HCOUNTEREN_HPM(n) COUNT_HPM(n)

// Hypervisor Time Delta Register

// Hypervisor Trap Value Register
//#define HTVAL_

// Hypervisor Trap Instruction Register
#define HTINST_READ 0x3000
#define HTINST_WRITE 0x3020

// Hypervisor Guest Address Translation and Protection Register
#define HGATP_PPN ATP_PPN
#define HGATP_VMID (0x3fffUL << 44)	// Virtual Machine Identifier
#define HGATP_VMID_N(n) ((n) << 44)
#define HGATP_MODE ATP_MODE

// Virtual Supervisor Status Register
#define VSSTATUS_SIE STATUS_SIE
#define VSSTATUS_SPIE STATUS_SPIE
#define VSSTATUS_UBE STATUS_UBE
#define VSSTATUS_SPP STATUS_SPP
#define VSSTATUS_VS STATUS_VS
#define VSSTATUS_FS STATUS_FS
#define VSSTATUS_XS STATUS_XS
#define VSSTATUS_SUM STATUS_SUM
#define VSSTATUS_MXR STATUS_MXR
#define VSSTATUS_UXL STATUS_UXL
#define VSSTATUS_SD STATUS_SD

// Virtual Supervisor Trap Vector Base Address Register

// Virtual Supervisor Interrupt Pending Register
#define VSIP_SSIP INT_SSI
#define VSIP_STIP INT_STI
#define VSIP_SEIP INT_SEI

// Virtual Supervisor Interrupt Enable Register
#define VSIE_SSIE INT_SSI
#define VSIE_STIE INT_STI
#define VSIE_SEIE INT_SEI

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
