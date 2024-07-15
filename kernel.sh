#!/bin/bash

# use built-in OpenSBI to run image
build_dir="./build/"

/home/marko/shit/qemu-8.0.2/build/qemu-system-riscv64 \
	-nographic \
	-machine virt \
	-kernel ${build_dir}hypervisor \
	-cpu rv64 \
	-smp $(grep DTB_NR_CPUS src/include/dtb.h | cut -d' ' -f3) \
	-m 1G \
	-gdb tcp::1111 \
	-S #-machine dumpdtb=dtb/double_uart.dts

        #-device pci-serial,chardev=pciserial -chardev socket,id=pciserial,host=127.0.0.1,port=4444,server=on \

# when changing smp value, do not forget to set DTB_NR_CPUS in include/dtb.h

# pciserial is found by (where 0x00021b36 is from here https://www.qemu.org/docs/master/specs/pci-serial.html)
# set $satp=0
# set $hgatp=0
# find 0x30000000, +0x10000000, 0x00021b36
