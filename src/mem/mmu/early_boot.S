.section ".text", "ax"

.globl early_mem_prot_init
early_mem_prot_init:
	# bootstrap pgtables + enter VAS
	ret
