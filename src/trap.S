.globl strapvec
.align 4
strapvec:
	sret

.globl mtrapvec
.align 4
mtrapvec:
	mret
