#!/bin/bash

# https://elinux.org/Device_Tree_Usage

src_dir=../src/
build_dir=../build/

/home/marko/shit/qemu-8.0.2/build/qemu-system-riscv64 \
	-nographic \
	-machine virt \
	-kernel ${build_dir}hypervisor \
	-cpu rv64 \
	-smp $(grep DTB_NR_CPUS ${src_dir}include/dtb.h | cut -d' ' -f3) \
	-m 1G \
	-machine dumpdtb=riscv64-virt.dtb \
	-device pci-serial,chardev=pciserial -chardev socket,id=pciserial,host=localhost,port=4444,server \

dtc riscv64-virt.dtb > riscv64-virt.dts

	#-device virtio-serial-device -chardev pty,id=serial3 -device virtconsole,chardev=serial3 \
