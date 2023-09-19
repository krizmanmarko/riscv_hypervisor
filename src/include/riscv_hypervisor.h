#ifndef RISCV_HYPERVISOR_H
#define RISCV_HYPERVISOR_H

#include "types.h"
#include "riscv.h"

// Hypervisor extension
// uses S-mode registers for exception, interrupt and address translation subsystems
// offers new HS-mode registers (hstatus, hideleg, hedeleg)
// offers extra set of VS-mode registers
// modifies M-mode registers

#define STATUS_GVA (1UL << 38)	// Guest Virtual Address

#define INT_HYPERVISOR_SW 0
#define INT_VIRTUAL_SUPERVISOR_SW 2
#define INT_HYPERVISOR_TIM 4
#define INT_VIRTUAL_SUPERVISOR_TIM 6
#define INT_HYPERVISOR_EXT 8
#define INT_VIRTUAL_SUPERVISOR_EXT 10
#define INT_HYPERVISOR_GUEST_EXT 12	// check out section on hip, hie, hvip

#define INT_HSSI (1UL << INT_HYPERVISOR_SW)
#define INT_VSSI (1UL << INT_VIRTUAL_SUPERVISOR_SW)
#define INT_HSTI (1UL << INT_HYPERVISOR_TIM)
#define INT_VSTI (1UL << INT_VIRTUAL_SUPERVISOR_TIM)
#define INT_HSEI (1UL << INT_HYPERVISOR_EXT)
#define INT_VSEI (1UL << INT_VIRTUAL_SUPERVISOR_EXT)
#define INT_SGEI (1UL << INT_HYPERVISOR_GUEST_EXT)	// check out hypervisor mip

#define EXC_ECALL_FROM_HS 9
#define EXC_ECALL_FROM_VS 10
#define EXC_INSTRUCTION_GUEST_PAGE_FAULT 20
#define EXC_LOAD_GUEST_PAGE_FAULT 21
#define EXC_VIRTUAL_INSTRUCTION 22
#define EXC_STORE_OR_AMO_GUEST_PAGE_FAULT 23


// Machine Status Registers
#define MSTATUS_GVA STATUS_GVA
#define MSTATUS_MPV (1UL << 39)		// Machine Previous Virtualization Mode
// modifies behavior of: TSR, TVM, TW, MPRV fields

// Machine Exception Delegation Register
#define MEDELEG_ECALL_FROM_HS (1UL << EXC_ECALL_FROM_HS)
#define MEDELEG_ECALL_FROM_VS (1UL << EXC_ECALL_FROM_VS)
#define MEDELEG_INSTRUCTION_GUEST_PAGE_FAULT (1UL << EXC_INSTRUCTION_GUEST_PAGE_FAULT)
#define MEDELEG_LOAD_GUEST_PAGE_FAULT (1UL << EXC_LOAD_GUEST_PAGE_FAULT)
#define MEDELEG_VIRTUAL_INSTRUCTION (1UL << EXC_VIRTUAL_INSTRUCTION)
#define MEDELEG_STORE_OR_AMO_GUEST_PAGE_FAULT (1UL << EXC_STORE_OR_AMO_GUEST_PAGE_FAULT)

// Machine Interrupt Delegation Register
#define MIDELEG_VSSI INT_VSSI	// read-only 1
#define MIDELEG_VSTI INT_VSTI	// read-only 1
#define MIDELEG_VSEI INT_VSEI	// read-only 1
#define MIDELEG_SGEI INT_SGEI

// Machine Interrupt Pending Register
#define MIP_VSSIP INT_VSSI
#define MIP_VSTIP INT_VSTI
#define MIP_VSEIP INT_VSEI
#define MIP_SGEIP INT_SGEI

// Machine Interrupt Enable Register
#define MIE_VSSIE INT_VSSI
#define MIE_VSTIE INT_VSTI
#define MIE_VSEIE INT_VSEI
#define MIE_SGEIE INT_SGEI

// Machine Second Trap Value Register
#define R_MTVAL2(uint64ptr) asm volatile("csrr %0, mtval2" : "=r" (*uint64ptr))
#define W_MTVAL2(value) asm volatile("csrw mtval2, %0" : : "r" (value))

// Machine Trap Instruction Register
#define R_MTINST(uint64ptr) asm volatile("csrr %0, mtinst" : "=r" (*uint64ptr))
#define W_MTINST(value) asm volatile("csrw mtinst, %0" : : "r" (value))

// Hypervisor Status Register
#define R_HSTATUS(uint64ptr) asm volatile("csrr %0, hstatus" : "=r" (*uint64ptr))
#define W_HSTATUS(value) asm volatile("csrw hstatus, %0" : : "r" (value))
#define HSTATUS_VSBE (1UL << 5)		// Virtual Supervisor Big Endianness
#define HSTATUS_GVA STATUS_GVA		// Guest Virtual Address
#define HSTATUS_SPV (1UL << 7)		// Supervisor Previous Virtualization Mode
#define HSTATUS_SPVP (1UL << 8)		// Supervisor Previous Virtual Privilege
#define HSTATUS_HU (1UL << 9)		// Hypervisor in U-mode
#define HSTATUS_VGEIN (0x3fUL << 12)	// Virtual Guest External Interrupt Number
#define HSTATUS_VTVM (1UL << 20)	// Virtual Trap Virtual Memory
#define HSTATUS_VTW (1UL << 21)		// Virtual Timout Wait
#define HSTATUS_VTSR (1UL << 22)	// Virtual Trap SRET
#define HSTATUS_VSXL (0b11UL << 32)	// effective VSXLEN

// Hypervisor Exception Delegation Register
#define R_HEDELEG(uint64ptr) asm volatile("csrr %0, hedeleg" : "=r" (*uint64ptr))
#define W_HEDELEG(value) asm volatile("csrw hedeleg, %0" : : "r" (value))
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
#define R_HIDELEG(uint64ptr) asm volatile("csrr %0, hideleg" : "=r" (*uint64ptr))
#define W_HIDELEG(value) asm volatile("csrw hideleg, %0" : : "r" (value))
#define HIDELEG_HSSI INT_HSSI
#define HIDELEG_VSSI INT_VSSI
#define HIDELEG_MSI INT_MSI
#define HIDELEG_HSTI INT_HSTI
#define HIDELEG_VSTI INT_VSTI
#define HIDELEG_MTI INT_MTI
#define HIDELEG_HSEI INT_HSEI
#define HIDELEG_VSEI INT_VSEI
#define HIDELEG_MEI INT_MEI
#define HIDELEG_SGEI INT_SGEI

// Hypervisor Virtual Interrupt Pending Register
#define R_HVIP(uint64ptr) asm volatile("csrr %0, hvip" : "=r" (*uint64ptr))
#define W_HVIP(value) asm volatile("csrw hvip, %0" : : "r" (value))
#define HVIP_VSSIP INT_VSSI
#define HVIP_VSTIP INT_VSTI
#define HVIP_VSEIP INT_VSEI

// Hypervisor Interrupt Pending Register
#define R_HIP(uint64ptr) asm volatile("csrr %0, hip" : "=r" (*uint64ptr))
#define W_HIP(value) asm volatile("csrw hip, %0" : : "r" (value))
#define HIP_VSSIP INT_VSSI
#define HIP_VSTIP INT_VSTI
#define HIP_VSEIP INT_VSEI
#define HIP_SGEIP INT_SGEI

// Hypervisor Interrupt Enable Register
#define R_HIE(uint64ptr) asm volatile("csrr %0, hie" : "=r" (*uint64ptr))
#define W_HIE(value) asm volatile("csrw hie, %0" : : "r" (value))
#define HIE_VSSIE INT_VSSI
#define HIE_VSTIE INT_VSTI
#define HIE_VSEIE INT_VSEI
#define HIE_SGEIE INT_SGEI

// Hypervisor Guest External Interrupt Pending Register
#define R_HGEIP(uint64ptr) asm volatile("csrr %0, hgeip" : "=r" (*uint64ptr))

// Hypervisor Guest External Interrupt Enable Register
#define R_HGEIE(uint64ptr) asm volatile("csrr %0, hgeie" : "=r" (*uint64ptr))
#define W_HGEIE(value) asm volatile("csrw hgeie, %0" : : "r" (value))

// Hypervisor Environment Configuration Register
#define R_HENVCFG(uint64ptr) asm volatile("csrr %0, henvcfg" : "=r" (*uint64ptr))
#define W_HENVCFG(value) asm volatile("csrw henvcfg, %0" : : "r" (value))
#define HENVCFG_FIOM ENVCFG_FIOM

// Hypervisor Counter-Enable Register
#define R_HCOUNTEREN(uint32ptr) asm volatile("csrr %0, hcounteren" : "=r" (*uint32ptr))
#define W_HCOUNTEREN(value) asm volatile("csrw hcounteren, %0" : : "r" (value))
#define HCOUNTEREN_CY COUNT_CY
#define HCOUNTEREN_TM COUNT_TM
#define HCOUNTEREN_IR COUNT_IR
#define HCOUNTEREN_HPM3 COUNT_HPM3
#define HCOUNTEREN_HPM4 COUNT_HPM4
#define HCOUNTEREN_HPM5 COUNT_HPM5
#define HCOUNTEREN_HPM6 COUNT_HPM6
#define HCOUNTEREN_HPM7 COUNT_HPM7
#define HCOUNTEREN_HPM8 COUNT_HPM8
#define HCOUNTEREN_HPM9 COUNT_HPM9
#define HCOUNTEREN_HPM10 COUNT_HPM10
#define HCOUNTEREN_HPM11 COUNT_HPM11
#define HCOUNTEREN_HPM12 COUNT_HPM12
#define HCOUNTEREN_HPM13 COUNT_HPM13
#define HCOUNTEREN_HPM14 COUNT_HPM14
#define HCOUNTEREN_HPM15 COUNT_HPM15
#define HCOUNTEREN_HPM16 COUNT_HPM16
#define HCOUNTEREN_HPM17 COUNT_HPM17
#define HCOUNTEREN_HPM18 COUNT_HPM18
#define HCOUNTEREN_HPM19 COUNT_HPM19
#define HCOUNTEREN_HPM20 COUNT_HPM20
#define HCOUNTEREN_HPM21 COUNT_HPM21
#define HCOUNTEREN_HPM22 COUNT_HPM22
#define HCOUNTEREN_HPM23 COUNT_HPM23
#define HCOUNTEREN_HPM24 COUNT_HPM24
#define HCOUNTEREN_HPM25 COUNT_HPM25
#define HCOUNTEREN_HPM26 COUNT_HPM26
#define HCOUNTEREN_HPM27 COUNT_HPM27
#define HCOUNTEREN_HPM28 COUNT_HPM28
#define HCOUNTEREN_HPM29 COUNT_HPM29
#define HCOUNTEREN_HPM30 COUNT_HPM30
#define HCOUNTEREN_HPM31 COUNT_HPM31

// Hypervisor Time Delta Register
#define R_HTIMEDELTA(uint64ptr) asm volatile("csrr %0, htimedelta" : "=r" (*uint64ptr))
#define W_HTIMEDELTA(value) asm volatile("csrw htimedelta, %0" : : "r" (value))

// Hypervisor Trap Value Register
#define R_HTVAL(uint64ptr) asm volatile("csrr %0, htval" : "=r" (*uint64ptr))
#define W_HTVAL(value) asm volatile("csrw htval, %0" : : "r" (value))

// Hypervisor Trap Instruction Register
#define R_HTINST(uint64ptr) asm volatile("csrr %0, htinst" : "=r" (*uint64ptr))
#define W_HTINST(value) asm volatile("csrw htinst, %0" : : "r" (value))

// Hypervisor Guest Address Translation and Protection Register
#define R_HGATP(uint64ptr) asm volatile("csrr %0, hgatp" : "=r" (*uint64ptr))
#define W_HGATP(value) asm volatile("csrw hgatp, %0" : : "r" (value))
#define HGATP_PPN ATP_PPN
#define HGATP_VMID (0x3fffUL << 44)	// Virtual Machine Identifier
#define HGATP_MODE ATP_MODE


// Virtual Supervisor Status Register
#define R_VSSTATUS(uint64ptr) asm volatile("csrr %0, vsstatus" : "=r" (*uint64ptr))
#define W_VSSTATUS(value) asm volatile("csrw vsstatus, %0" : : "r" (value))
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
#define R_VSTVEC(uint64ptr) asm volatile("csrr %0, vstvec" : "=r" (*uint64ptr))
#define W_VSTVEC(value) asm volatile("csrw vstvec, %0" : : "r" (value))

// Virtual Supervisor Interrupt Pending Register
#define R_VSIP(uint64ptr) asm volatile("csrr %0, vsip" : "=r" (*uint64ptr))
#define W_VSIP(value) asm volatile("csrw vsip, %0" : : "r" (value))
#define VSIP_SSIP INT_SSI
#define VSIP_STIP INT_STI
#define VSIP_SEIP INT_SEI

// Virtual Supervisor Interrupt Enable Register
#define R_VSIE(uint64ptr) asm volatile("csrr %0, vsie" : "=r" (*uint64ptr))
#define W_VSIE(value) asm volatile("csrw vsie, %0" : : "r" (value))
#define VSIE_SSIE INT_SSI
#define VSIE_STIE INT_STI
#define VSIE_SEIE INT_SEI

// Virtual Supervisor Scratch Register
#define R_VSSCRATCH(uint64ptr) asm volatile("csrr %0, vsscratch" : "=r" (*uint64ptr))
#define W_VSSCRATCH(value) asm volatile("csrw vsscratch, %0" : : "r" (value))

// Virtual Supervisor Exception Program Counter Register
#define R_VSEPC(uint64ptr) asm volatile("csrr %0, vsepc" : "=r" (*uint64ptr))
#define W_VSEPC(value) asm volatile("csrw vsepc, %0" : : "r" (value))

// Virtual Supervisor Cause Register
#define R_VSCAUSE(uint64ptr) asm volatile("csrr %0, vscause" : "=r" (*uint64ptr))
#define W_VSCAUSE(value) asm volatile("csrw vscause, %0" : : "r" (value))

// Virtual Supervisor Trap Value Register
#define R_VSTVAL(uint64ptr) asm volatile("csrr %0, vstval" : "=r" (*uint64ptr))
#define W_VSTVAL(value) asm volatile("csrw vstval, %0" : : "r" (value))

// Virtual Supervisor Address Translation and Protection Register
#define R_VSATP(uint64ptr) asm volatile("csrr %0, vsatp" : "=r" (*uint64ptr))
#define W_VSATP(value) asm volatile("csrw vsatp, %0" : : "r" (value))
#define VSATP_PPN ATP_PPN
#define VSATP_ASID ATP_ASID
#define VSATP_MODE ATP_MODE


#endif // RISCV_HYPERVISOR_H
