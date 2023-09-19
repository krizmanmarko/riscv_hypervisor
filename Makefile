
qemu: ./build/hypervisor
	@./machine.sh

./build/hypervisor:
	cd src && make
