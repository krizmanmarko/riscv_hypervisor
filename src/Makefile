OBJFILES=asm_boot.o asm_trap.o main.o start.o uart.o printf.o cpu.o asm_spinlock.o spinlock.o trap.o vmem.o kmem.o
TESTS=printf_test.o spinlock_test.o asm_exception_tests.o
OBJFILES+= $(TESTS)
OBJECTS=$(patsubst %, $(BUILD_DIR)%, $(OBJFILES))

INCDIR=./include

CFLAGS=-Wall -ggdb -I$(INCDIR)
#CFLAGS+= -Werror
# isolate from host environment
CFLAGS+=-ffreestanding -fno-common -nodefaultlibs -nostdlib
# position independent code
CFLAGS+=-mcmodel=medany

PAGE_SIZE=4096
LDFLAGS=-nostdlib
LDFLAGS+= -z common-page-size=$(PAGE_SIZE) -z max-page-size=$(PAGE_SIZE)

CROSS_COMPILE=/home/marko/shit/riscv-gnu-toolchain-build/bin/riscv64-unknown-linux-gnu-
cc=$(CROSS_COMPILE)gcc
ld=$(CROSS_COMPILE)ld
as=$(CROSS_COMPILE)as
objcopy=$(CROSS_COMPILE)objcopy


BUILD_DIR=../build/

.PHONY: all clean

all: clean $(BUILD_DIR)hypervisor tags

tags:
	ctags -R .

# watch out! linkerscript.ld must be before $(OBJECTS) because it is an argument for -T switch
$(BUILD_DIR)hypervisor: $(BUILD_DIR)linkerscript.ld $(OBJECTS)
	$(ld) $(LDFLAGS) -T $^ -o $@

$(BUILD_DIR)linkerscript.ld: linkerscript.c
	gcc -I$(INCDIR) -P -E $^ -o $@

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

$(BUILD_DIR)asm_%.o: test/%.S
	$(cc) $(CFLAGS) -c $^ -o $@

clean:
	rm -f tags
	rm -f $(BUILD_DIR)*
