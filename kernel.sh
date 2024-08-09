#!/bin/bash

# use built-in OpenSBI to run image
build_dir="./build/"

/home/marko/shit/qemu-8.0.2/build/qemu-system-riscv64 \
	-nographic \
	-monitor tcp::6666,server=on,nowait \
	-machine virt \
	-global virtio-mmio.force-legacy=false \
	-device pci-serial,chardev=pciserial -chardev socket,id=pciserial,host=127.0.0.1,port=1337,server=on \
	-drive file=src/guest/xv6-riscv-guest/fs.img,if=none,format=raw,id=x0 \
	-device virtio-blk-device,drive=x0,bus=virtio-mmio-bus.0 \
	-drive file=src/guest/xv6-riscv-guest/fs2.img,if=none,format=raw,id=x1 \
	-device virtio-blk-device,drive=x1,bus=virtio-mmio-bus.1 \
	-kernel ${build_dir}hypervisor \
	-cpu rv64 \
	-smp $(grep DTB_NR_CPUS src/include/dtb.h | cut -d' ' -f3) \
	-m 256M \
	-gdb tcp::1111 \
	-S


# pciserial is found by (where 0x00021b36 is from here https://www.qemu.org/docs/master/specs/pci-serial.html)
# set $satp=0
# set $hgatp=0
# find 0x30000000, +0x10000000, 0x00021b36
