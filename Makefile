
qemu:
	@./machine.sh

debug:
	@riscv64-unknown-linux-gnu-gdb build/hypervisor -ex 'target remote localhost:1234' -ex 'disp/3i $$pc' -ex 'b * setup'
