OBJFILES=asm_boot.o asm_trap.o main.o start.o uart.o printf.o test.o cpu.o asm_spinlock.o spinlock.o
OBJECTS=$(patsubst %, $(BUILD_DIR)%, $(OBJFILES))

INCDIR=./include

CFLAGS=-Wall -ggdb -I$(INCDIR)
#CFLAGS+= -Werror
# isolate from host environment
CFLAGS+=-ffreestanding -fno-common -nodefaultlibs
# position independent code
CFLAGS+=-mcmodel=medany

CROSS_COMPILE=/home/marko/shit/riscv-gnu-toolchain-build/bin/riscv64-unknown-linux-gnu-
cc=$(CROSS_COMPILE)gcc
ld=$(CROSS_COMPILE)ld
as=$(CROSS_COMPILE)as
objcopy=$(CROSS_COMPILE)objcopy


BUILD_DIR=../build/

.PHONY: all clean driver lib

all: clean driver lib $(BUILD_DIR)hypervisor tags

tags:
	ctags -R .

$(BUILD_DIR)hypervisor: $(OBJECTS)
	$(ld) -T linker.ld $^ -o $@

$(BUILD_DIR)asm_%.o: %.S
	$(cc) $(CFLAGS) -c $^ -o $@

$(BUILD_DIR)%.o: %.c
	$(cc) $(CFLAGS) -c $^ -o $@

$(BUILD_DIR)%.o: driver/%.c
	$(cc) $(CFLAGS) -c $^ -o $@

$(BUILD_DIR)%.o: lib/%.c
	$(cc) $(CFLAGS) -c $^ -o $@

$(BUILD_DIR)asm_%.o: lib/%.S
	$(cc) $(CFLAGS) -c $^ -o $@

$(BUILD_DIR)%.o: test/%.c
	$(cc) $(CFLAGS) -c $^ -o $@

clean:
	rm -f tags
	rm -f $(BUILD_DIR)*
