
qemu:
	@./machine.sh

debug:
	@riscv64-unknown-elf-gdb build/hypervisor \
		-ex 'target remote localhost:1234' \
		-ex 'disp/3i $$pc' \
		-ex 'set scheduler-locking on' \
		-ex 'source src/helper/pgtable.py'
