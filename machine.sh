build_dir="/home/marko/home/schola/diploma/risc-v/build/"
/home/marko/shit/qemu-8.0.2/build/qemu-system-riscv64 \
	-nographic \
	-machine virt \
	-bios ${build_dir}hypervisor \
	-smp 1 \
	-m 1G \
	-s -S
