
qemu: ./build/hypervisor
#	@echo "/home/marko/shit/riscv-gnu-toolchain-build/bin/riscv64-unknown-linux-gnu-gdb /home/marko/home/schola/diploma/risc-v/build/hypervisor.elf -ex 'target remote localhost:1234' -ex 'disp/3i $pc'"
	@./machine.sh

./build/hypervisor:
	@cd src && make
