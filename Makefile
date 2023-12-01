
kernel:
	@./kernel.sh

debug:
	@riscv64-unknown-elf-gdb ./build/hypervisor \
		-ex 'target extended-remote localhost:1111' \
		-ex 'disp/3i $$pc' \
		-ex 'set scheduler-locking on' \
		-ex 'source src/helper/pgtable.py'
