#ifndef RISCV_H
#define RISCV_H // target: riscv64 (qemu-virt)


#include "types.h"

// WARL - write any, read legal
// WLRL - write legal, read legal
// WPRI - reserved writes preserve values, reads ignore values (ignored)

#define M_MODE 3
#define S_MODE 1
#define U_MODE 0

// All mode common register fields
#define STATUS_SIE (1UL << 1)		// Supervisor Interrupt Enable
#define STATUS_MIE (1UL << 3)		// Machine Interrupt Enable
#define STATUS_SPIE (1UL << 5)		// Supervisor Previous Interrupt Enable
#define STATUS_UBE (1UL << 6)		// User is big-endian (no effect on fetch)
#define STATUS_MPIE (1UL << 7)		// Machine Previous Interrupt Enable
#define STATUS_SPP (1UL << 8)		// Supervisor Previous Privilege
#define STATUS_SPP_S (S_MODE << 8)
#define STATUS_SPP_U (U_MODE << 8)
#define STATUS_VS (0b11UL << 9)		// Vector extension state
#define STATUS_MPP (0b11UL << 11)	// Machine Previous Privilege
#define STATUS_MPP_M (M_MODE << 11)
#define STATUS_MPP_S (S_MODE << 11)
#define STATUS_MPP_U (U_MODE << 11)
#define STATUS_FS (0b11UL << 13)	// Floating-point Unit State
#define STATUS_XS (0b11UL << 15)	// User Mode extensions
#define STATUS_MPRV (1UL << 17)		// Modify Privilege (set effective privilege)
#define STATUS_SUM (1UL << 18)		// Permit Supervisor User Memory access
#define STATUS_MXR (1UL << 19)		// Make Executable Readable
#define STATUS_TVM (1UL << 20)		// Trap Virtual Memory
#define STATUS_TW (1UL << 21)		// Timeout Wait
#define STATUS_TSR (1UL << 22)		// Trap SRET
#define STATUS_UXL (0b11UL << 32)	// User XLEN
#define STATUS_SXL (0b11UL << 34)	// Supervisor XLEN
#define STATUS_SBE (1UL << 36)		// Supervisor is big-endian (no effect on fetch)
#define STATUS_MBE (1UL << 37)		// Machine is big-endian (no effect on fetch)
#define STATUS_SD (1UL << 63)		// Something dirty (in FS, VS or XS)

#define TVEC_MODE_DIRECT 0
#define TVEC_MODE_VECTORED 1

#define INT_SUPERVISOR_SW 1
#define INT_MACHINE_SW 3
#define INT_SUPERVISOR_TIM 5
#define INT_MACHINE_TIM 7
#define INT_SUPERVISOR_EXT 9
#define INT_MACHINE_EXT 11

#define INT_SSI (1UL << INT_SUPERVISOR_SW)
#define INT_MSI (1UL << INT_MACHINE_SW)
#define INT_STI (1UL << INT_SUPERVISOR_TIM)
#define INT_MTI (1UL << INT_MACHINE_TIM)
#define INT_SEI (1UL << INT_SUPERVISOR_EXT)
#define INT_MEI (1UL << INT_MACHINE_EXT)

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

#define COUNT_CY (1UL << 0)	// Cycle
#define COUNT_TM (1UL << 1)	// Time
#define COUNT_IR (1UL << 2)	// Instret
#define COUNT_HPM3 (1UL << 3)	// Hardware Performance Monitor
#define COUNT_HPM4 (1UL << 4)	// Hardware Performance Monitor
#define COUNT_HPM5 (1UL << 5)	// Hardware Performance Monitor
#define COUNT_HPM6 (1UL << 6)	// Hardware Performance Monitor
#define COUNT_HPM7 (1UL << 7)	// Hardware Performance Monitor
#define COUNT_HPM8 (1UL << 8)	// Hardware Performance Monitor
#define COUNT_HPM9 (1UL << 9)	// Hardware Performance Monitor
#define COUNT_HPM10 (1UL << 10)	// Hardware Performance Monitor
#define COUNT_HPM11 (1UL << 11)	// Hardware Performance Monitor
#define COUNT_HPM12 (1UL << 12)	// Hardware Performance Monitor
#define COUNT_HPM13 (1UL << 13)	// Hardware Performance Monitor
#define COUNT_HPM14 (1UL << 14)	// Hardware Performance Monitor
#define COUNT_HPM15 (1UL << 15)	// Hardware Performance Monitor
#define COUNT_HPM16 (1UL << 16)	// Hardware Performance Monitor
#define COUNT_HPM17 (1UL << 17)	// Hardware Performance Monitor
#define COUNT_HPM18 (1UL << 18)	// Hardware Performance Monitor
#define COUNT_HPM19 (1UL << 19)	// Hardware Performance Monitor
#define COUNT_HPM20 (1UL << 20)	// Hardware Performance Monitor
#define COUNT_HPM21 (1UL << 21)	// Hardware Performance Monitor
#define COUNT_HPM22 (1UL << 22)	// Hardware Performance Monitor
#define COUNT_HPM23 (1UL << 23)	// Hardware Performance Monitor
#define COUNT_HPM24 (1UL << 24)	// Hardware Performance Monitor
#define COUNT_HPM25 (1UL << 25)	// Hardware Performance Monitor
#define COUNT_HPM26 (1UL << 26)	// Hardware Performance Monitor
#define COUNT_HPM27 (1UL << 27)	// Hardware Performance Monitor
#define COUNT_HPM28 (1UL << 28)	// Hardware Performance Monitor
#define COUNT_HPM29 (1UL << 29)	// Hardware Performance Monitor
#define COUNT_HPM30 (1UL << 30)	// Hardware Performance Monitor
#define COUNT_HPM31 (1UL << 31)	// Hardware Performance Monitor

#define CAUSE_INT (1UL << 63)

#define ENVCFG_FIOM (1UL << 0)		// Fence of I/O Implies Memory

#define ATP_PPN (0xfffffffffffUL << 0)	// 44-bit physical page number
#define ATP_ASID (0xffffUL << 44)	// Virtual Machine Identifier
#define ATP_MODE (0b1111UL << 60)
#define ATP_MODE_BARE (0b0000UL << 60)
#define ATP_MODE_Sv39 (0b0100UL << 60)


// Machine ISA Register (WARL)
#define R_MISA(uint64ptr) asm volatile("csrr %0, misa" : "=r" (*uint64ptr))
#define W_MISA(value) asm volatile("csrw misa, %0" : : "r" (value));
#define MISA_EXT_A (1UL << 0)	// Atomic extension
#define MISA_EXT_C (1UL << 2)	// Compressed extension
#define MISA_EXT_D (1UL << 3)	// Double-precision floating-point extension
#define MISA_EXT_F (1UL << 5)	// Single-precision floating-point extension
#define MISA_EXT_H (1UL << 7)	// Hypervisor extension
#define MISA_EXT_I (1UL << 8)	// RV32I/64I/128I base ISA
#define MISA_EXT_M (1UL << 12)	// Integer Multiply/Divide extension
#define MISA_EXT_S (1UL << 18)	// Supervisor mode implemented
#define MISA_EXT_U (1UL << 20)	// User mode implemented

// Machine Vendor, Architecture, Implementation and Hart ID Registers
#define R_MVENDORID(uint32ptr) asm volatile("csrr %0, mvendorid" : "=r" (uint32ptr))
#define R_MARCHID(uint64ptr) asm volatile("csrr %0, marchid" : "=r" (*uint64ptr))
#define R_MIMPID(uint64ptr) asm volatile("csrr %0, mimpid" : "=r" (*uint64ptr))
#define R_MHARTID(uint64ptr) asm volatile("csrr %0, mhartid" : "=r" (*uint64ptr))

// Machine Status Register (WARL)
#define R_MSTATUS(uint64ptr) asm volatile("csrr %0, mstatus" : "=r" (*uint64ptr))
#define W_MSTATUS(value) asm volatile("csrw mstatus, %0" : : "r" (value))
#define MSTATUS_SIE STATUS_SIE
#define MSTATUS_MIE STATUS_MIE
#define MSTATUS_SPIE STATUS_SPIE
#define MSTATUS_UBE STATUS_UBE
#define MSTATUS_MPIE STATUS_MPIE
#define MSTATUS_SPP STATUS_SPP
#define MSTATUS_VS STATUS_VS
#define MSTATUS_MPP STATUS_MPP
#define MSTATUS_FS STATUS_FS
#define MSTATUS_XS STATUS_XS
#define MSTATUS_MPRV STATUS_MPRV
#define MSTATUS_SUM STATUS_SUM
#define MSTATUS_MXR STATUS_MXR
#define MSTATUS_TVM STATUS_TVM
#define MSTATUS_TW STATUS_TW
#define MSTATUS_TSR STATUS_TSR
#define MSTATUS_UXL STATUS_UXL
#define MSTATUS_SXL STATUS_SXL
#define MSTATUS_SBE STATUS_SBE
#define MSTATUS_MBE STATUS_MBE
#define MSTATUS_SD STATUS_SD

// Machine Trap-Vector Base-Address Register (WARL)
#define R_MTVEC(uint64ptr) asm volatile("csrr %0, mtvec" : "=r" (*uint64ptr))
#define W_MTVEC(value) asm volatile("csrw mtvec, %0" : : "r" (value))

// Machine Interrupt Pending Register
#define R_MIP(uint64ptr) asm volatile("csrr %0, mip" : "=r" (*uint64ptr))
#define W_MIP(value) asm volatile("csrw mip, %0" : : "r" (value))
#define MIP_SSIP INT_SSI
#define MIP_MSIP INT_MSI
#define MIP_STIP INT_STI
#define MIP_MTIP INT_MTI
#define MIP_SEIP INT_SEI
#define MIP_MEIP INT_MEI

// Machine Interrupt Enable Register
#define R_MIE(uint64ptr) asm volatile("csrr %0, mie" : "=r" (*uint64ptr))
#define W_MIE(value) asm volatile("csrw mie, %0" : : "r" (value))
#define MIE_SSIE INT_SSI
#define MIE_MSIE INT_MSI
#define MIE_STIE INT_STI
#define MIE_MTIE INT_MTI
#define MIE_SEIE INT_SEI
#define MIE_MEIE INT_MEI

// Machine Exception Delegation Register
#define R_MEDELEG(uint64ptr) asm volatile("csrr %0, medeleg" : "=r" (*uint64ptr))
#define W_MEDELEG(value) asm volatile("csrw medeleg, %0" : : "r" (value))
#define MEDELEG_INSTRUCTION_ADDR_MISALIGNED (1UL << EXC_INSTRUCTION_ADDR_MISALIGNED)
#define MEDELEG_INSTRUCTION_ACCESS_FAULT (1UL << EXC_INSTRUCTION_ACCESS_FAULT)
#define MEDELEG_ILLEGAL_INSTRUCTION (1UL << EXC_ILLEGAL_INSTRUCTION)
#define MEDELEG_BREAKPOINT (1UL << EXC_BREAKPOINT)
#define MEDELEG_LOAD_ADDR_MISALIGNED (1UL << EXC_LOAD_ADDR_MISALIGNED)
#define MEDELEG_LOAD_ACCESS_FAULT (1UL << EXC_LOAD_ACCESS_FAULT)
#define MEDELEG_STORE_OR_AMO_ADDRESS_MISALIGNED (1UL << EXC_STORE_OR_AMO_ADDRESS_MISALIGNED)
#define MEDELEG_STORE_OR_AMO_ACCESS_FAULT (1UL << EXC_STORE_OR_AMO_ACCESS_FAULT)
#define MEDELEG_ECALL_FROM_U (1UL << EXC_ECALL_FROM_U)
#define MEDELEG_ECALL_FROM_S (1UL << EXC_ECALL_FROM_S)
#define MEDELEG_ECALL_FROM_M (1UL << EXC_ECALL_FROM_M)		// does nothing in qemu-virt, but can be set
#define MEDELEG_INSTRUCTION_PAGE_FAULT (1UL << EXC_INSTRUCTION_PAGE_FAULT)
#define MEDELEG_LOAD_PAGE_FAULT (1UL << EXC_LOAD_PAGE_FAULT)
#define MEDELEG_STORE_OR_AMO_PAGE_FAULT (1UL << EXC_STORE_OR_AMO_PAGE_FAULT)

// Machine Interrupt Delegation Register
#define R_MIDELEG(uint64ptr) asm volatile("csrr %0, mideleg" : "=r" (*uint64ptr))
#define W_MIDELEG(value) asm volatile("csrw mideleg, %0" : : "r" (value))
#define MIDELEG_SSI INT_SSI
#define MIDELEG_MSI INT_MSI
#define MIDELEG_STI INT_STI
#define MIDELEG_MTI INT_MTI
#define MIDELEG_SEI INT_SEI
#define MIDELEG_MEI INT_MEI

// Machine Counter-Enable Register
#define R_MCOUNTEREN(uint32ptr) asm volatile("csrr %0, mcounteren" : "=r" (uint32ptr));
#define W_MCOUNTEREN(value) asm volatile("csrw mcounteren, %0" : : "r" ((uint32) value));
#define MCOUNTEREN_CY COUNT_CY
#define MCOUNTEREN_TM COUNT_TM
#define MCOUNTEREN_IR COUNT_IR
#define MCOUNTEREN_HPM3 COUNT_HPM3
#define MCOUNTEREN_HPM4 COUNT_HPM4
#define MCOUNTEREN_HPM5 COUNT_HPM5
#define MCOUNTEREN_HPM6 COUNT_HPM6
#define MCOUNTEREN_HPM7 COUNT_HPM7
#define MCOUNTEREN_HPM8 COUNT_HPM8
#define MCOUNTEREN_HPM9 COUNT_HPM9
#define MCOUNTEREN_HPM10 COUNT_HPM10
#define MCOUNTEREN_HPM11 COUNT_HPM11
#define MCOUNTEREN_HPM12 COUNT_HPM12
#define MCOUNTEREN_HPM13 COUNT_HPM13
#define MCOUNTEREN_HPM14 COUNT_HPM14
#define MCOUNTEREN_HPM15 COUNT_HPM15
#define MCOUNTEREN_HPM16 COUNT_HPM16
#define MCOUNTEREN_HPM17 COUNT_HPM17
#define MCOUNTEREN_HPM18 COUNT_HPM18
#define MCOUNTEREN_HPM19 COUNT_HPM19
#define MCOUNTEREN_HPM20 COUNT_HPM20
#define MCOUNTEREN_HPM21 COUNT_HPM21
#define MCOUNTEREN_HPM22 COUNT_HPM22
#define MCOUNTEREN_HPM23 COUNT_HPM23
#define MCOUNTEREN_HPM24 COUNT_HPM24
#define MCOUNTEREN_HPM25 COUNT_HPM25
#define MCOUNTEREN_HPM26 COUNT_HPM26
#define MCOUNTEREN_HPM27 COUNT_HPM27
#define MCOUNTEREN_HPM28 COUNT_HPM28
#define MCOUNTEREN_HPM29 COUNT_HPM29
#define MCOUNTEREN_HPM30 COUNT_HPM30
#define MCOUNTEREN_HPM31 COUNT_HPM31

// Machine Counter-Inhibit CSR
#define R_MCOUNTINHIBIT(uint32ptr) asm volatile("csrr %0, mcountinhibit" : "=r" (uint32ptr));
#define W_MCOUNTINHIBIT(value) asm volatile("csrw mcountinhibit, %0" : : "r" ((uint32) value));
#define MCOUNTINHIBIT_CY COUNT_CY
#define MCOUNTINHIBIT_IR COUNT_IR
#define MCOUNTINHIBIT_HPM3 COUNT_HPM3
#define MCOUNTINHIBIT_HPM4 COUNT_HPM4
#define MCOUNTINHIBIT_HPM5 COUNT_HPM5
#define MCOUNTINHIBIT_HPM6 COUNT_HPM6
#define MCOUNTINHIBIT_HPM7 COUNT_HPM7
#define MCOUNTINHIBIT_HPM8 COUNT_HPM8
#define MCOUNTINHIBIT_HPM9 COUNT_HPM9
#define MCOUNTINHIBIT_HPM10 COUNT_HPM10
#define MCOUNTINHIBIT_HPM11 COUNT_HPM11
#define MCOUNTINHIBIT_HPM12 COUNT_HPM12
#define MCOUNTINHIBIT_HPM13 COUNT_HPM13
#define MCOUNTINHIBIT_HPM14 COUNT_HPM14
#define MCOUNTINHIBIT_HPM15 COUNT_HPM15
#define MCOUNTINHIBIT_HPM16 COUNT_HPM16
#define MCOUNTINHIBIT_HPM17 COUNT_HPM17
#define MCOUNTINHIBIT_HPM18 COUNT_HPM18
#define MCOUNTINHIBIT_HPM19 COUNT_HPM19
#define MCOUNTINHIBIT_HPM20 COUNT_HPM20
#define MCOUNTINHIBIT_HPM21 COUNT_HPM21
#define MCOUNTINHIBIT_HPM22 COUNT_HPM22
#define MCOUNTINHIBIT_HPM23 COUNT_HPM23
#define MCOUNTINHIBIT_HPM24 COUNT_HPM24
#define MCOUNTINHIBIT_HPM25 COUNT_HPM25
#define MCOUNTINHIBIT_HPM26 COUNT_HPM26
#define MCOUNTINHIBIT_HPM27 COUNT_HPM27
#define MCOUNTINHIBIT_HPM28 COUNT_HPM28
#define MCOUNTINHIBIT_HPM29 COUNT_HPM29
#define MCOUNTINHIBIT_HPM30 COUNT_HPM30
#define MCOUNTINHIBIT_HPM31 COUNT_HPM31

// Machine Scratch
#define R_MSCRATCH(uint64ptr) asm volatile("csrr %0, mscratch" : "=r" (*uint64ptr))
#define W_MSCRATCH(value) asm volatile("csrw mscratch, %0" : : "r" (value))

// Machine Exception Program Counter
#define R_MEPC(uint64ptr) asm volatile("csrr %0, mepc" : "=r" (*uint64ptr))
#define W_MEPC(value) asm volatile("csrw mepc, %0" : : "r" (value))

// Machine Cause Register
#define R_MCAUSE(uint64ptr) asm volatile("csrr %0, mcause" : "=r" (*uint64ptr))
#define W_MCAUSE(value) asm volatile("csrw mcause, %0" : : "r" (value))

// Machine Trap Value Register
#define R_MTVAL(uint64ptr) asm volatile("csrr %0, mtval" : "=r" (*uint64ptr))
#define W_MTVAL(value) asm volatile("csrw mtval, %0" : : "r" (value))

// Machine Configuration Pointer Register
#define R_MCONFIGPTR(uint64ptr) asm volatile("csrr %0, mconfigptr" : "=r" (*uint64ptr))

// Machine Environment Configuration Register
#define R_MENVCFG(uint64ptr) asm volatile("csrr %0, menvcfg" : "=r" (*uint64ptr))
#define W_MENVCFG(value) asm volatile("csrw menvcfg, %0" : : "r" (value))
#define MENVCFG_FIOM ENVCFG_FIOM

// Physical Memory Protection Registers
#define W_PMPCFG0(value) asm volatile("csrw pmpcfg0, %0" : : "r" (value))
#define W_PMPADDR0(value) asm volatile("csrw pmpaddr0, %0" : : "r" (value))
#define W_PMPADDR1(value) asm volatile("csrw pmpaddr1, %0" : : "r" (value))
#define W_PMPADDR2(value) asm volatile("csrw pmpaddr2, %0" : : "r" (value))
#define W_PMPADDR3(value) asm volatile("csrw pmpaddr3, %0" : : "r" (value))
#define W_PMPADDR4(value) asm volatile("csrw pmpaddr4, %0" : : "r" (value))
#define W_PMPADDR5(value) asm volatile("csrw pmpaddr5, %0" : : "r" (value))
#define W_PMPADDR6(value) asm volatile("csrw pmpaddr6, %0" : : "r" (value))
#define W_PMPADDR7(value) asm volatile("csrw pmpaddr7, %0" : : "r" (value))
#define PMPCFG_R (1UL << 0)		// Read
#define PMPCFG_W (1UL << 1)		// Write
#define PMPCFG_X (1UL << 2)		// Execute
#define PMPCFG_A (0b11UL << 3)		// Address Matching Mode
#define PMPCFG_A_TOR (0b01UL << 3)	// Address Matching Mode - Top of Range
#define PMPCFG_A_OFF (0b00UL << 3)	// Address Matching Mode - Off
#define PMPCFG_L (1UL << 7)		// Locking


// Supervisor Status Register
#define R_SSTATUS(uint64ptr) asm volatile("csrr %0, sstatus" : "=r" (*uint64ptr))
#define W_SSTATUS(value) asm volatile("csrw sstatus, %0" : : "r" (value))
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
#define R_STVEC(uint64ptr) asm volatile("csrr %0, stvec" : "=r" (*uint64ptr))
#define W_STVEC(value) asm volatile("csrw stvec, %0" : : "r" (value))

// Supervisor Interrupt Pending Register
#define R_SIP(uint64ptr) asm volatile("csrr %0, sip" : "=r" (*uint64ptr))
#define W_SIP(value) asm volatile("csrw sip, %0" : : "r" (value))
#define SIP_SSIP INT_SSI
#define SIP_STIP INT_STI
#define SIP_SEIP INT_SEI

// Supervisor Interrupt Enable Register
#define R_SIE(uint64ptr) asm volatile("csrr %0, sie" : "=r" (*uint64ptr))
#define W_SIE(value) asm volatile("csrw sie, %0" : : "r" (value))
#define SIE_SSIE INT_SSI
#define SIE_STIE INT_STI
#define SIE_SEIE INT_SEI

// Supervisor Counter-Enable Register
#define R_SCOUNTEREN(uint32ptr) asm volatile("csrr %0, scounteren" : "=r" (uint32ptr));
#define W_SCOUNTEREN(value) asm volatile("csrw scounteren, %0" : : "r" ((uint32) value));
#define SCOUNTEREN_CY COUNT_CY
#define SCOUNTEREN_TM COUNT_TM
#define SCOUNTEREN_IR COUNT_IR
#define SCOUNTEREN_HPM3 COUNT_HPM3
#define SCOUNTEREN_HPM4 COUNT_HPM4
#define SCOUNTEREN_HPM5 COUNT_HPM5
#define SCOUNTEREN_HPM6 COUNT_HPM6
#define SCOUNTEREN_HPM7 COUNT_HPM7
#define SCOUNTEREN_HPM8 COUNT_HPM8
#define SCOUNTEREN_HPM9 COUNT_HPM9
#define SCOUNTEREN_HPM10 COUNT_HPM10
#define SCOUNTEREN_HPM11 COUNT_HPM11
#define SCOUNTEREN_HPM12 COUNT_HPM12
#define SCOUNTEREN_HPM13 COUNT_HPM13
#define SCOUNTEREN_HPM14 COUNT_HPM14
#define SCOUNTEREN_HPM15 COUNT_HPM15
#define SCOUNTEREN_HPM16 COUNT_HPM16
#define SCOUNTEREN_HPM17 COUNT_HPM17
#define SCOUNTEREN_HPM18 COUNT_HPM18
#define SCOUNTEREN_HPM19 COUNT_HPM19
#define SCOUNTEREN_HPM20 COUNT_HPM20
#define SCOUNTEREN_HPM21 COUNT_HPM21
#define SCOUNTEREN_HPM22 COUNT_HPM22
#define SCOUNTEREN_HPM23 COUNT_HPM23
#define SCOUNTEREN_HPM24 COUNT_HPM24
#define SCOUNTEREN_HPM25 COUNT_HPM25
#define SCOUNTEREN_HPM26 COUNT_HPM26
#define SCOUNTEREN_HPM27 COUNT_HPM27
#define SCOUNTEREN_HPM28 COUNT_HPM28
#define SCOUNTEREN_HPM29 COUNT_HPM29
#define SCOUNTEREN_HPM30 COUNT_HPM30
#define SCOUNTEREN_HPM31 COUNT_HPM31

// Supervisor Scratch Register
#define R_SSCRATCH(uint64ptr) asm volatile("csrr %0, sscratch" : "=r" (*uint64ptr))
#define W_SSCRATCH(value) asm volatile("csrw sscratch, %0" : : "r" (value))

// Supervisor Exception Program Counter
#define R_SEPC(uint64ptr) asm volatile("csrr %0, sepc" : "=r" (*uint64ptr))
#define W_SEPC(value) asm volatile("csrw sepc, %0" : : "r" (value))

// Supervisor Cause Register
#define R_SCAUSE(uint64ptr) asm volatile("csrr %0, scause" : "=r" (*uint64ptr))
#define W_SCAUSE(value) asm volatile("csrw scause, %0" : : "r" (value))

// Supervisor Trap Value Register
#define R_STVAL(uint64ptr) asm volatile("csrr %0, stval" : "=r" (*uint64ptr))
#define W_STVAL(value) asm volatile("csrw stval, %0" : : "r" (value))

// Supervisor Environment Configuration Register
#define R_SENVCFG(uint64ptr) asm volatile("csrr %0, senvcfg" : "=r" (*uint64ptr))
#define W_SENVCFG(value) asm volatile("csrw senvcfg, %0" : : "r" (value))
#define SENVCFG_FIOM ENVCFG_FIOM

// Supervisor Address Translation and Protection Register
#define R_SATP(uint64ptr) asm volatile("csrr %0, satp" : "=r" (*uint64ptr))
#define W_SATP(value) asm volatile("csrw satp, %0" : : "r" (value))
#define SATP_PPN ATP_PPN
#define SATP_ASID ATP_ASID
#define SATP_MODE ATP_MODE


#endif // RISCV_H
