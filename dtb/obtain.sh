#!/bin/bash

build_dir="../build/"
/home/marko/shit/qemu-8.0.2/build/qemu-system-riscv64 \
	-nographic \
	-machine virt \
	-kernel ${build_dir}hypervisor \
	-smp 3 \
	-m 1G \
	-s -S \
	-machine dumpdtb=riscv64-virt.dts

#/home/marko/shit/qemu-8.0.2/build/qemu-system-riscv64 \
#	-machine virt \
#	-smp 3 \
#	-m 512M \
#	-machine dumpdtb=riscv64-virt.dts

dtc riscv64-virt.dts > dtb.txt
