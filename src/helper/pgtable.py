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

def get_pgdirs(pa, level):
	pgdir = get_pgdir(pa)
	pgdirs = [None] * 512
	for i, pte in enumerate(pgdir):
		if pte == None:
			continue
		if pte & pte_rwx:
			pgdirs[i] = pte		# found Mega/Giga page
			continue
		if pte & pte_v:
			if level > 0:
				pgdirs[i] = get_pgdirs(pte2pa(pte), level - 1)
			else:
				pgdirs[i] = pte	# found page
	return pgdirs

def parse_pte(pte):
	pa = pte2pa(pte)
	r = 'r' if pte & (1 << 1) else '-'
	w = 'w' if pte & (1 << 2) else '-'
	x = 'x' if pte & (1 << 3) else '-'
	u = 'u' if pte & (1 << 4) else '-'
	g = 'g' if pte & (1 << 4) else '-'
	return (pa, r, w, x, u, g)

def get_va(i=None, j=None, k=None):
	if i == None:
		print('nope')
		exit(1)
	va = i << 9 << 9 << 12
	if va & (1 << 38):
		va |= 0xffffffc000000000

	if j == None:
		return va
	va |= j << 9 << 12

	if k == None:
		return va
	va |= k << 12
	return va

def print_pte(root, pte, i=None, j=None, k=None):
	va = get_va(i, j, k)
	pa, r, w, x, u, g = parse_pte(pte)
	size = None
	if i != None and j == None and k == None:
		size = '1G'
		j = -1
		k = -1
	elif i != None and j != None and k == None:
		size = '2M'
		k = -1
	elif i != None and j != None and k != None:
		size = '4K'
	else:
		print('something went wrong...')
		exit(1)
	print(f'{root:16x}[{i:3d}][{j:3d}][{k:3d}] {size} {r}{w}{x} {u}{g}: {hex(va)} -> {hex(pa)}')

def print_pgtable_sv39(pa):
	l2 = get_pgdirs(pa, 2)
	root = pa
	for i, l2entry in enumerate(l2):
		if isinstance(l2entry, int):
			print_pte(root, l2entry, i)
		elif l2entry != None:
			for j, l1entry in enumerate(l2[i]):
				if isinstance(l1entry, int):
					print_pte(root, l1entry, i, j)
				elif l1entry != None:
					for k, l0entry in enumerate(l2[i][j]):
						if l0entry != None:
							print_pte(root, l0entry, i, j, k)

# supply the root page table address
def pgtable(address):
	# disable memory protection, so I can traverse page table
	satp = int(gdb.execute('info registers satp', to_string=True).split()[1], 16)
	hgatp = int(gdb.execute('info registers hgatp', to_string=True).split()[1], 16)
	gdb.execute('set $satp = 0')
	gdb.execute('set $hgatp = 0')

	pa = address
	try:
		print_pgtable_sv39(pa)
	finally:
		# on exit reenable satp and hgatp
		gdb.execute('set $satp = ' + str(satp))
		gdb.execute('set $hgatp = ' + str(hgatp))
