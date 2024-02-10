/* WARNING: locks do not disable interrupts */

.section ".text", "ax"

.globl hstrapvec
.globl vm_entry

.macro VM_ENTER
	csr swap x31, sscratch, x31
	ld x1, 0(x31)
	ld x2, 8(x31)
	ld x3, 16(x31)
	ld x4, 24(x31)
	ld x5, 32(x31)
	ld x6, 40(x31)
	ld x7, 48(x31)
	ld x8, 56(x31)
	ld x9, 64(x31)
	ld x10, 72(x31)
	ld x11, 80(x31)
	ld x12, 88(x31)
	ld x13, 96(x31)
	ld x14, 104(x31)
	ld x15, 112(x31)
	ld x16, 120(x31)
	ld x17, 128(x31)
	ld x18, 136(x31)
	ld x19, 144(x31)
	ld x20, 152(x31)
	ld x21, 160(x31)
	ld x22, 168(x31)
	ld x23, 176(x31)
	ld x24, 184(x31)
	ld x25, 192(x31)
	ld x26, 200(x31)
	ld x27, 208(x31)
	ld x28, 216(x31)
	ld x29, 224(x31)
	ld x30, 232(x31)
.endm

.macro VM_EXIT
	csrrw x31, sscratch, x31
	sd x1, 0(x31)
	sd x2, 8(x31)
	sd x3, 16(x31)
	sd x4, 24(x31)
	sd x5, 32(x31)
	sd x6, 40(x31)
	sd x7, 48(x31)
	sd x8, 56(x31)
	sd x9, 64(x31)
	sd x10, 72(x31)
	sd x11, 80(x31)
	sd x12, 88(x31)
	sd x13, 96(x31)
	sd x14, 104(x31)
	sd x15, 112(x31)
	sd x16, 120(x31)
	sd x17, 128(x31)
	sd x18, 136(x31)
	sd x19, 144(x31)
	sd x20, 152(x31)
	sd x21, 160(x31)
	sd x22, 168(x31)
	sd x23, 176(x31)
	sd x24, 184(x31)
	sd x25, 192(x31)
	sd x26, 200(x31)
	sd x27, 208(x31)
	sd x28, 216(x31)
	sd x29, 224(x31)
	sd x30, 232(x31)
	
	// sscratch holds &vcpu
	// fill vcpu
.endm

.balign 4
hstrapvec:
	csrrw sp, sscratch, sp
	addi sp, sp, -256
	sd x0, 0*8(sp)
	sd x1, 1*8(sp)
	sd x2, 2*8(sp)
	sd x3, 3*8(sp)
	sd x4, 4*8(sp)
	sd x5, 5*8(sp)
	sd x6, 6*8(sp)
	sd x7, 7*8(sp)
	sd x8, 8*8(sp)
	sd x9, 9*8(sp)
	sd x10, 10*8(sp)
	sd x11, 11*8(sp)
	sd x12, 12*8(sp)
	sd x13, 13*8(sp)
	sd x14, 14*8(sp)
	sd x15, 15*8(sp)
	sd x16, 16*8(sp)
	sd x17, 17*8(sp)
	sd x18, 18*8(sp)
	sd x19, 19*8(sp)
	sd x20, 20*8(sp)
	sd x21, 21*8(sp)
	sd x22, 22*8(sp)
	sd x23, 23*8(sp)
	sd x24, 24*8(sp)
	sd x25, 25*8(sp)
	sd x26, 26*8(sp)
	sd x27, 27*8(sp)
	sd x28, 28*8(sp)
	sd x29, 29*8(sp)
	sd x30, 30*8(sp)
	sd x31, 31*8(sp)

	# distinguish interrupt from exception
	csrr a0, scause
	srli t0, a0, 63

	beqz t0, 1f
	call hs_interrupt_handler
	j 2f

1:	call hs_exception_handler

2:
	ld x31, 31*8(sp)
	ld x30, 30*8(sp)
	ld x29, 29*8(sp)
	ld x28, 28*8(sp)
	ld x27, 27*8(sp)
	ld x26, 26*8(sp)
	ld x25, 25*8(sp)
	ld x24, 24*8(sp)
	ld x23, 23*8(sp)
	ld x22, 22*8(sp)
	ld x21, 21*8(sp)
	ld x20, 20*8(sp)
	ld x19, 19*8(sp)
	ld x18, 18*8(sp)
	ld x17, 17*8(sp)
	ld x16, 16*8(sp)
	ld x15, 15*8(sp)
	ld x14, 14*8(sp)
	ld x13, 13*8(sp)
	ld x12, 12*8(sp)
	ld x11, 11*8(sp)
	ld x10, 10*8(sp)
	ld x9, 9*8(sp)
	ld x8, 8*8(sp)
	ld x7, 7*8(sp)
	ld x6, 6*8(sp)
	ld x5, 5*8(sp)
	ld x4, 4*8(sp)
	ld x3, 3*8(sp)
	ld x2, 2*8(sp)
	ld x1, 1*8(sp)
	ld x0, 0*8(sp)
	addi sp, sp, 256
	csrrw sp, sscratch, sp
	sret

