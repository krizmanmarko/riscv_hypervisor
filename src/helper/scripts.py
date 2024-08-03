#!/usr/bin/python

import socket

def recvuntil(sock, delimiter):
	data = b''
	while not data.endswith(delimiter):
		chunk = sock.recv(1)
		if not chunk:
			raise Exception("Connection closed before delimiter was found")
		data += chunk
	return data

def get_val(addr=0x9fe62000):
	client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	client.connect(('localhost', 6666))
	recvuntil(client, b'(qemu) ')
	client.send(b'xp/gx ' + str(addr).encode() + b'\n')
	recvuntil(client, b': ')
	val = int(recvuntil(client, b'\n')[:-2].decode(), 16)
	client.close()
	return val

def watch(addr=0x9fe62000):
	i = 0
	val = get_val(addr)
	while val == 0x4141414141414141:
		gdb.execute('c')
		i += 1
		print(f'iteration: {i}')
		val = get_val(addr)
		if i == 30:
			break
	print(f'val hasn\'t changed yet: {hex(val)}')
	gdb.execute('disp/i $pc')
	while val == 0x4141414141414141:
		gdb.execute('ni')
		i += 1
		print(f'iteration: {i}')
		val = get_val(addr)
	print(f'val changed: {hex(val)}')
