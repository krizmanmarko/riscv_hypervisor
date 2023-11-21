#!/usr/bin/python3
# this file is used to print Sv39 page tables in gdb

PTE_V = 1
PTE_RWX = 2 | 4 | 8

def pa2pte(pa):
	return (pa >> 12) << 10

def pte2pa(pa):
	return (pa >> 10) << 12

def get_pgdir(pa):
	pgdir_text = gdb.execute('x/512gx ' + str(pa), to_string=True)
	pgdir = []
	for line in pgdir_text.split('\n'):
		for entry in line.split()[1:]:
			pgdir.append(int(entry, 16))
	return pgdir

def get_pgdirs(pa):
	pgdir = get_pgdir(pa)
	pgdirs = [None] * 512
	for i, pte in enumerate(pgdir):
		if pte == None:
			continue
	 	if pte & PTE_RWX:
			pgdirs[i] = pte2pa(pte)		# actual page
			continue
		if pte & PTE_V:
			pa = pte2pa(pte)
			pgdirs[i] = get_pgdirs(pa)
	return pgdirs

def print_pgtable(pa):
	l2 = get_pgdirs(pa)
	root = pa
	for i, l2entry in enumerate(l2):
		if isinstance(l2entry, int):
			va = i << 9 << 9 << 12
			if va >= 0x4000000000:
				va |= 0xffffffc000000000
			pa = l2entry
			print('%16x[%3d] 1G: 0x%x -> 0x%x' % (root, i, va, pa))
		elif l2entry != None:
			for j, l1entry in enumerate(l2[i]):
				if isinstance(l1entry, int):
					va = i << 9 << 9 << 12
					va |= j << 9 << 12
					if va >= 0x4000000000:
						va |= 0xffffffc000000000
					pa = l1entry
					print('%16x[%3d][%3d] 2M: 0x%x -> 0x%x' % (root, i, j, va, pa))
				elif l1entry != None:
					for k, l0entry in enumerate(l2[i][j]):
						if l0entry != None:
							va = i << 9 << 9 << 12
							va |= j << 9 << 12
							va |= k << 12
							if va >= 0x4000000000:
								va |= 0xffffffc000000000
							pa = l0entry
							print('%16x[%3d][%3d][%3d] 4K: 0x%x -> 0x%x' % (root, i, j, k, va, pa))

# if address is None then satp is read for pgtable
# otherwise you supply the root page table address
def pgtable(address=None):
	satp = int(gdb.execute('info registers satp', to_string=True).split()[1], 16)
	# disable memory protection, so I can traverse page table
	gdb.execute('set $satp = 0')

	pa = address
	if pa == None:
		pa = satp & ~(0b11 << 62)	# disable mode sv39
		pa <<= 12			# get phys address

	try:
		print_pgtable(pa)
	finally:
		# on exit reenable satp
		gdb.execute('set $satp = ' + str(satp))
