build_dir="/home/marko/home/schola/diploma/risc-v/build/"
/home/marko/shit/qemu-8.0.2/build/qemu-system-riscv64 \
	-nographic \
	-machine virt \
	-bios none \
	-kernel ${build_dir}hypervisor \
	-smp 3 \
	-m 512M \
	-s # -S
