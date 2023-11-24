#!/usr/bin/python
# this file is used to print Sv39 page tables in gdb

pte_v = 1
pte_rwx = 2 | 4 | 8

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
		if pte & pte_rwx:
			pgdirs[i] = pte2pa(pte)		# actual page
			continue
		if pte & pte_v:
			pa = pte2pa(pte)
			pgdirs[i] = get_pgdirs(pa)
	return pgdirs

def print_pgtable_sv39(pa):
	l2 = get_pgdirs(pa)
	root = pa
	for i, l2entry in enumerate(l2):
		if isinstance(l2entry, int):
			va = i << 9 << 9 << 12
			if va & (1 << 38):
				va |= 0xffffffc000000000
			pa = l2entry
			print('%16x[%3d] 1G: 0x%x -> 0x%x' % (root, i, va, pa))
		elif l2entry != None:
			for j, l1entry in enumerate(l2[i]):
				if isinstance(l1entry, int):
					va = i << 9 << 9 << 12
					va |= j << 9 << 12
					if va & (1 << 38):
						va |= 0xffffffc000000000
					pa = l1entry
					print('%16x[%3d][%3d] 2M: 0x%x -> 0x%x' % (root, i, j, va, pa))
				elif l1entry != None:
					for k, l0entry in enumerate(l2[i][j]):
						if l0entry != None:
							va = i << 9 << 9 << 12
							va |= j << 9 << 12
							va |= k << 12
							if va & (1 << 38):
								va |= 0xffffffc000000000
							pa = l0entry
							print('%16x[%3d][%3d][%3d] 4K: 0x%x -> 0x%x' % (root, i, j, k, va, pa))

def print_pgtable_sv48(pa):
	l3 = get_pgdirs(pa)
	root = pa
	for i, l3entry in enumerate(l3):
		if isinstance(l3entry, int):
			va = i << 9 << 9 << 9 << 12
			if va & (1 << 47):
				va |= 0xffff000000000000
			pa = l3entry
			print('%16x[%3d] 512G: 0x%x -> 0x%x' % (root, i, va, pa))
		elif l3entry != None:
			for j, l2entry in enumerate(l3[i]):
				if isinstance(l2entry, int):
					va = i << 9 << 9 << 9 << 12
					va |= j << 9 << 9 << 12
					if va & (1 << 47):
						va |= 0xffff000000000000
					pa = l2entry
					print('%16x[%3d][%3d] 1G: 0x%x -> 0x%x' % (root, i, j, va, pa))
				elif l2entry != None:
					for k, l1entry in enumerate(l3[i][j]):
						if isinstance(l1entry, int):
							va = i << 9 << 9 << 9 << 12
							va |= j << 9 << 9 << 12
							va |= k << 9 << 12
							if va & (1 << 47):
								va |= 0xffff000000000000
							pa = l1entry
							print('%16x[%3d][%3d][%3d] 2M: 0x%x -> 0x%x' % (root, i, j, k, va, pa))
						elif l1entry != None:
							for l, l0entry in enumerate(l3[i][j][k]):
								if l0entry != None:
									va = i << 9 << 9 << 9 << 12
									va |= j << 9 << 9 << 12
									va |= k << 9 << 12
									va |= l << 12
									if va & (1 << 47):
										va |= 0xffff000000000000
									pa = l0entry
									print('sv48 haha')
									print(l0entry)
									#print('%16x[%3d][%3d][%3d][%3d] 4K: 0x%x -> 0x%x' % (root, i, j, k, l, va, pa))

def print_pgtable_sv57(pa):
	l4 = get_pgdirs(pa)
	root = pa
	for i, l4entry in enumerate(l4):
		if isinstance(l4entry, int):
			va = i << 9 << 9 << 9 << 9 << 12
			if va & (1 << 56):
				va |= 0xff00000000000000
			pa = l4entry
			print('%16x[%3d] 256T: 0x%x -> 0x%x' % (root, i, va, pa))
		elif l4entry != None:
			for j, l3entry in enumerate(l4[i]):
				if isinstance(l3entry, int):
					va = i << 9 << 9 << 9 << 9 << 12
					va |= j << 9 << 9 << 9 << 12
					if va & (1 << 56):
						va |= 0xff00000000000000
					pa = l3entry
					print('%16x[%3d][%3d] 512G: 0x%x -> 0x%x' % (root, i, j, va, pa))
				elif l3entry != None:
					for k, l2entry in enumerate(l4[i][j]):
						if isinstance(l2entry, int):
							va = i << 9 << 9 << 9 << 9 << 12
							va |= j << 9 << 9 << 9 << 12
							va |= k << 9 << 9 << 12
							if va & (1 << 56):
								va |= 0xff00000000000000
							pa = l2entry
							print('%16x[%3d][%3d][%3d] 1G: 0x%x -> 0x%x' % (root, i, j, k, va, pa))
						elif l2entry != None:
							for l, l1entry in enumerate(l4[i][j][k]):
								if isinstance(l1entry, int):
									va = i << 9 << 9 << 9 << 9 << 12
									va |= j << 9 << 9 << 9 << 12
									va |= k << 9 << 9 << 12
									va |= l << 9 << 12
									if va & (1 << 56):
										va |= 0xff00000000000000
									pa = l1entry
									print('%16x[%3d][%3d][%3d][%3d] 2M: 0x%x -> 0x%x' % (root, i, j, k, l, va, pa))
								elif l1entry != None:
									for m, l0entry in enumerate(l4[i][j][k][l]):
										if l0entry != None:
											va = i << 9 << 9 << 9 << 9 << 12
											va |= j << 9 << 9 << 9 << 12
											va |= k << 9 << 9 << 12
											va |= l << 9 << 12
											va |= m << 12
											if va & (1 << 56):
												va |= 0xff00000000000000
											pa = l0entry
											print('%16x[%3d][%3d][%3d][%3d][%3d] 4K: 0x%x -> 0x%x' % (root, i, j, k, l, m, va, pa))
											

# if address is None then satp is read for pgtable
# otherwise you supply the root page table address
def pgtable(address=None, mode=0x8):
	satp = int(gdb.execute('info registers satp', to_string=True).split()[1], 16)
	# disable memory protection, so I can traverse page table
	gdb.execute('set $satp = 0')

	pa = address
	if pa == None:
		mode = satp >> 60
		pa = satp & ~(0xf << 60)	# disable mode
		pa = satp & ~(0xffff << 44)	# disable asid
		pa <<= 12			# get phys address

	try:
		if mode == 0x8:
			print_pgtable_sv39(pa)
		elif mode == 0x9:
			print_pgtable_sv48(pa)
		elif mode == 0xa:
			print_pgtable_sv57(pa)
	finally:
		# on exit reenable satp
		gdb.execute('set $satp = ' + str(satp))
