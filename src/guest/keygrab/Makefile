## CONFIG
NAME=keygrab
CROSS_COMPILE = /home/marko/shit/riscv-gnu-toolchain-from-source/mybuild/bin/riscv64-unknown-elf-
BUILD_DIR = ./build
CODE_DIRS = core driver
## END CONFIG

# build directory creation
$(shell mkdir -p $(BUILD_DIR))
$(foreach DIR,$(CODE_DIRS),$(shell mkdir -p $(BUILD_DIR)/$(DIR)))

CC = $(CROSS_COMPILE)gcc
OBJCOPY = $(CROSS_COMPILE)objcopy

DEPFLAGS = -MP -MD
INCLUDES = -I./include -I$(BUILD_DIR)
LDFLAGS = -z common-page-size=4096 -z max-page-size=4096

CFLAGS = -W -Wall -ggdb $(INCLUDES) $(LDFLAGS)
CFLAGS += -ffreestanding -fno-common -nodefaultlibs -nostdlib
# from linux source arch/riscv/mm/init.c line 806
# 1) It should use PC-relative addressing for accessing kernel symbols.
#    To achieve this we always use GCC cmodel=medany.
CFLAGS += -std=gnu17 -march=rv64gch -mcmodel=medany -fno-pic

# spinlock.c, spinlock.S -> must generate different named object file
C_FILES = $(foreach DIR,$(CODE_DIRS),$(wildcard $(DIR)/*.c))
ASM_FILES = $(foreach DIR,$(CODE_DIRS),$(wildcard $(DIR)/*.S))
OBJECTS = $(patsubst %.c,$(BUILD_DIR)/%.o,$(C_FILES))
OBJECTS += $(patsubst %.S,$(BUILD_DIR)/%_asm.o,$(ASM_FILES))
DEPFILES = $(patsubst %.o,%.d,$(OBJECTS))

# compile(input, output)
define compile
	@$(CC) $(CFLAGS) $(DEPFLAGS) -c $1 -o $2
	@echo "[+] $1 -> $2"
endef

########################################################
# THIS IS directly EVALED so we need double $ somewhere
########################################################
# generate(subdir)
define generate
$(BUILD_DIR)/$1/%_asm.o: $1/%.S
	$(call compile,$$<,$$@)
$(BUILD_DIR)/$1/%.o: $1/%.c
	$(call compile,$$<,$$@)
endef
########################################################

default: $(BUILD_DIR)/$(NAME).bin tags

# generate rules for all subdirectories
$(foreach DIR, $(CODE_DIRS), $(eval $(call generate,$(DIR))))

tags:
	@ctags -R . $^
	@echo "[i] created tags"

$(BUILD_DIR)/$(NAME).bin: $(BUILD_DIR)/$(NAME)
	@$(OBJCOPY) -O binary $^ $@

# WARNING: linkerscript.ld must be first dependency because -T
$(BUILD_DIR)/$(NAME): linkerscript.ld $(OBJECTS)
	@$(CC) $(CFLAGS) -T $^ -o $@
	@echo "\e[32m[+] Successfuly built $@!\e[0m"

clean:
	@rm -f tags
	@rm -rf $(BUILD_DIR)/*
	@echo "[i] cleaned up"

-include $(DEPFILES)
