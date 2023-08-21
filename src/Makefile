CROSS_COMPILE=/home/marko/shit/riscv-gnu-toolchain-build/bin/riscv64-unknown-linux-gnu-

cc=$(CROSS_COMPILE)gcc -Wall -Werror -mcmodel=medany
ld=$(CROSS_COMPILE)ld
as=$(CROSS_COMPILE)as
objcopy=$(CROSS_COMPILE)objcopy

OBJFILES=boot.o trap.o main.o start.o
OBJECTS=$(patsubst %, $(BUILD_DIR)%, $(OBJFILES))

BUILD_DIR=../build/

.PHONY: all clean

all: $(BUILD_DIR)hypervisor
	ctags -R .

$(BUILD_DIR)hypervisor: $(OBJECTS)
	$(ld) -T linker.ld $^ -o $@

$(BUILD_DIR)%.o: %.S
	$(cc) -g -c $^ -o $@

$(BUILD_DIR)%.o: %.c
	$(cc) -g -c $^ -o $@

clean:
	rm tags
	rm -f $(BUILD_DIR)*
