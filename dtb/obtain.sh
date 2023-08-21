#!/bin/bash

/home/marko/shit/qemu-8.0.2/build/qemu-system-riscv64 \
	-machine virt \
	-smp 3 \
	-m 512M \
	-machine dumpdtb=riscv64-virt.dts

dtc riscv64-virt.dts > dtb.txt
