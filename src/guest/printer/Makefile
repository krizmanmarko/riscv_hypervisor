
# TODO: clean this mess up please
# a curious thing is going on here
# c files must be compiled before asm files since they need compiler help
# to generate correct offsets for c structs
# (this way we avoid hardcodeing offsets)
# i guess bao copied this https://stackoverflow.com/questions/8259447/give-structure-offset-attribute-to-assembler

OBJFILES=asm_main.o

OBJFILES+= $(TESTS)
OBJECTS=$(patsubst %, $(BUILD_DIR)%, $(OBJFILES))

#INCLUDES=-I ./include -I$(BUILD_DIR)

CFLAGS=-W -Wall -ggdb $(INCLUDES)
#CFLAGS+= -Werror
# isolate from host environment
CFLAGS+=-ffreestanding -fno-common -nodefaultlibs -nostdlib
# Position dependent code
CFLAGS+= -mcmodel=medany -fno-pic

PAGE_SIZE=4096
LDFLAGS=-nostdlib
LDFLAGS+= -z common-page-size=$(PAGE_SIZE) -z max-page-size=$(PAGE_SIZE)

#CROSS_COMPILE=/home/marko/shit/riscv-gnu-toolchain-build/bin/riscv64-unknown-linux-gnu-
CROSS_COMPILE=/home/marko/shit/toolchain-riscv-linux/bin/riscv64-unknown-elf-
cc=$(CROSS_COMPILE)gcc
ld=$(CROSS_COMPILE)ld
as=$(CROSS_COMPILE)as
objcopy=$(CROSS_COMPILE)objcopy


BUILD_DIR=./build/
IMGS_DIR=../imgs/

.PHONY: all clean

all: $(IMGS_DIR)guest.bin

$(IMGS_DIR)guest.bin: $(BUILD_DIR)guest.bin
	@cp -v $^ $@

$(BUILD_DIR)guest.bin: $(BUILD_DIR)guest
	@$(objcopy) -O binary $^ $@
	@echo "[+] Successfuly built $@!"

# watch out! linkerscript.ld must be before $(OBJECTS) because it is an argument for -T switch
$(BUILD_DIR)guest: $(OBJECTS)
	@$(ld) $(LDFLAGS) -T linkerscript.ld $^ -o $@

$(BUILD_DIR)asm_%.o: %.S
	@$(cc) $(CFLAGS) -c $< -o $@
	@echo "[+] $< -> $@"

clean:
	@rm -f $(BUILD_DIR)*
	@echo "[i] cleaned up"
