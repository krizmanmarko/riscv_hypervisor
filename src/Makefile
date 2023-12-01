
# TODO: clean this mess up please
# a curious thing is going on here
# c files must be compiled before asm files since they need compiler help
# to generate correct offsets for c structs
# (this way we avoid hardcodeing offsets)
# i guess bao copied this https://stackoverflow.com/questions/8259447/give-structure-offset-attribute-to-assembler

OBJFILES=trap.o main.o cpu.o kmem.o lock.o vmem.o printf.o uart.o string.o
OBJFILES+= asm_sboot.o asm_boot.o asm_trap.o asm_spinlock.o

TESTS=printf_test.o lock_test.o asm_exception_tests.o

OBJFILES+= $(TESTS)
OBJECTS=$(patsubst %, $(BUILD_DIR)%, $(OBJFILES))

INCLUDES=-I ./include -I$(BUILD_DIR)

CFLAGS=-W -Wall -ggdb $(INCLUDES)
CFLAGS+= -march=rv64gch
# g = imafd
# isolate from host environment
CFLAGS+=-ffreestanding -fno-common -nodefaultlibs -nostdlib
# PC-relative code
CFLAGS+= -mcmodel=medany -fno-pic

# from linux source arch/riscv/mm/init.c line 806
# 1) It should use PC-relative addressing for accessing kernel symbols.
#    To achieve this we always use GCC cmodel=medany.


PAGE_SIZE=4096
LDFLAGS=-nostdlib
LDFLAGS+= -z common-page-size=$(PAGE_SIZE) -z max-page-size=$(PAGE_SIZE)

CROSS_COMPILE=/home/marko/shit/riscv-gnu-toolchain-from-source/mybuild/bin/riscv64-unknown-elf-
cc=$(CROSS_COMPILE)gcc
ld=$(CROSS_COMPILE)ld
as=$(CROSS_COMPILE)as
objcopy=$(CROSS_COMPILE)objcopy


BUILD_DIR=../build/

.PHONY: all clean

all: clean tags $(BUILD_DIR)hypervisor

tags: $(BUILD_DIR)structs_in_asm.h
	@ctags -R . $^
	@echo "[i] created tags"

$(BUILD_DIR)hypervisor.bin: $(BUILD_DIR)hypervisor
	$(objcopy) -O binary $^ $@

# watch out! linkerscript.ld must be before $(OBJECTS) because it is an argument for -T switch
$(BUILD_DIR)hypervisor: $(BUILD_DIR)linkerscript.ld $(OBJECTS)
	@cd guest/printer && make
	@$(objcopy) --input binary --output elf64-littleriscv --rename-section .data=.vm_image_guest --set-section-flags .vm_image_guest=code,contents,alloc,load,readonly $(BUILD_DIR)printer/imgs/printer.bin $(BUILD_DIR)guest.o
	@$(ld) $(LDFLAGS) -T $^ -o $@ $(BUILD_DIR)guest.o
	@echo "\e[32m[+] Successfuly built $@!\e[0m"

$(BUILD_DIR)structs_in_asm.h: helper/structs_in_asm.c
	@$(cc) $(CFLAGS) -S $^ -o - \
		| awk '($$1 == "->") { print "#define " $$2 " (" $$3 ")" }' >$@
	@echo "[i] created structs_in_asm.h"

$(BUILD_DIR)linkerscript.ld: helper/linkerscript.c
	@gcc $(INCLUDES) -P -E $^ -o $@
	@echo "[+] $^ -> $@"

$(BUILD_DIR)asm_%.o: %.S $(BUILD_DIR)structs_in_asm.h
	@$(cc) $(CFLAGS) -c $< -o $@
	@echo "[+] $< -> $@"

$(BUILD_DIR)%.o: %.c
	@$(cc) $(CFLAGS) -c $^ -o $@
	@echo "[+] $^ -> $@"

$(BUILD_DIR)%.o: driver/%.c
	@$(cc) $(CFLAGS) -c $^ -o $@
	@echo "[+] $^ -> $@"

$(BUILD_DIR)%.o: lib/%.c
	@$(cc) $(CFLAGS) -c $^ -o $@
	@echo "[+] $^ -> $@"

$(BUILD_DIR)asm_%.o: lib/%.S $(BUILD_DIR)structs_in_asm.h
	@$(cc) $(CFLAGS) -c $< -o $@
	@echo "[+] $< -> $@"

$(BUILD_DIR)%.o: test/%.c
	@$(cc) $(CFLAGS) -c $^ -o $@
	@echo "[+] $^ -> $@"

$(BUILD_DIR)asm_%.o: test/%.S $(BUILD_DIR)structs_in_asm.h
	@$(cc) $(CFLAGS) -c $< -o $@
	@echo "[+] $< -> $@"

clean:
	@rm -f tags
	@rm -rf $(BUILD_DIR)*
	@echo "[i] cleaned up"
