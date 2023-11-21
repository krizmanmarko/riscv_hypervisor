
qemu:
	@./machine.sh

debug:
	# still using linux-gnu toolchain debugger since it supports python scripting
	# it also has riscv csr support
	@riscv64-unknown-linux-gnu-gdb build/hypervisor \
		-ex 'target remote localhost:1234' \
		-ex 'disp/3i $$pc' \
		-ex 'set scheduler-locking on' \
		-ex 'source src/helper/pgtable.py' \
		-ex 'pi pgtable()'

debug-unknown:
	@riscv64-unknown-elf-gdb build/hypervisor -ex 'target remote localhost:1234' -ex 'disp/3i $$pc'
