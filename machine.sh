build_dir="./build/"
/home/marko/shit/qemu-8.0.2/build/qemu-system-riscv64 \
	-nographic \
	-machine virt \
	-bios ${build_dir}hypervisor \
	-smp 1 \
	-m 1G \
	-gdb tcp::1111 \
	-S
