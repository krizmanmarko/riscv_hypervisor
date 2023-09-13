.globl strapvec
.align 4
strapvec:
	csrrw x31, sscratch, x31
	csrrw x31, sscratch, x31
	sret

.globl mtrapvec
.align 4
mtrapvec:
	j mtrapvec
