
kernel:
	@./kernel.sh

debug:
	@riscv64-unknown-elf-gdb -q ./build/hypervisor
