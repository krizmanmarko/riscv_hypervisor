#!/bin/bash

# use built-in OpenSBI to run image
build_dir="./build/"

#/home/marko/shit/qemu-8.2.0/build/qemu-system-riscv64 \
/home/marko/shit/qemu-8.0.2/build/qemu-system-riscv64 \
	-nographic \
	-machine virt \
	-kernel ${build_dir}hypervisor \
	-cpu rv64 \
	-smp 1 \
	-m 1G \
	-gdb tcp::1111 \
	#-S


# when changing smp value, do not forget to set DTB_NR_CPUS in include/dtb.h
