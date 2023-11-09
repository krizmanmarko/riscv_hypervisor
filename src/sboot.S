# S-mode entry point

# goals for this section:
#	setup bootstrap page table (with identity mapping)
#	enter vas
#	setup everything necessary to enter C code (stack, gp)

#include "structs_in_asm.h"

.section ".boot.text.s", "ax"
.globl setup

# a0: hart id
# a1: dtb
setup:
	# in case we trigger an exception in boot code
	la t0, sboot_fail
	csrw stvec, t0

	call init_bootstrap_pages
	call enter_vas
	call init_for_c
	j .

# va -> pa
# [0x80000000, dynamic] -> [0x80000000, dynamic] (just use gigapage (easy to unmap)
# [0xffffffc000000000, pa2va(dynamic)] -> [0x80000000, dynamic]
# [pa2va(dynamic), max_va] -> [dynamic, max_ram]

# i need identity mapping to perform switching vas
# i want to map hypervisor to 0xffffffc000000000, .+KERNEL_SIZE
# i want every cpu struct to map at 0xffffffc040000000, .+sizeof(struct cpu)
# i want dynamic to be at 0xffffffe000000000
# max_va 0xfffffff000000000
init_bootstrap_pages:
# we need
#	root pt (with following entries):
#		# kernel image
#		root[256][0]
#			root[256][0][0]
#			...
#			root[256][0][i] (depending on kernel size)
#		# cpu stack
#		root[257][0]
#			root[257][0][0]
#			...
#			root[257][0][j] (depending on cpu stack size)
#
#		# dynamic (root[384])
#		root[384][0]
#			root[384][0][0]
#			...
#			root[384][0][k] (depending on leftover memory)
	li a0, CPU_SIZE
	ret

enter_vas:
	ret

init_for_c:
	# store hartid in tp
	mv tp, a0

	# init stack
	# init gp
	ret

.align 4
sboot_fail:
	j .
