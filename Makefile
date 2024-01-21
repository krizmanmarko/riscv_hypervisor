
kernel:
	@./kernel.sh

debug:
	@riscv64-unknown-elf-gdb ./build/hypervisor \
		-ex 'target extended-remote localhost:1111' \
		-ex 'disp/3i $$pc' \
		-ex 'set scheduler-locking on' \
		-ex 'source src/helper/pgtable.py' \
		-ex 'maintenance packet qqemu.sstep=0x1'

# useful qemu flags in gdb
# maintenance packet qemu.sstepbits
# maintenance packet qqemu.PhyMemMode:0
# maintenance packet qqemu.PhyMemMode:1
