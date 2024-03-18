#!/bin/bash

# use built-in OpenSBI to run image
build_dir="./build/"

#/home/marko/shit/qemu-8.2.0/build/qemu-system-riscv64 \
/home/marko/shit/qemu-8.0.2/build/qemu-system-riscv64 \
	-nographic \
	-machine virt \
	-kernel ${build_dir}hypervisor \
	-cpu rv64 \
	-smp 2 \
	-m 1G \
	-gdb tcp::1111 \
	#-S
