.section ".text"

.globl strapvec
.align 4
strapvec:
	csrrw x31, sscratch, x31
	csrrw x31, sscratch, x31
	sret

.globl mtrapvec
.align 4
mtrapvec:
	mret

# for testing access fault exceptions
.section ".nop", "ax"

.globl access_fault_test_addr
access_fault_test_addr:
	nop
