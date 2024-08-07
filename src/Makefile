## CONFIG
NAME=hypervisor
CROSS_COMPILE = /home/marko/shit/riscv-gnu-toolchain-from-source/mybuild/bin/riscv64-unknown-elf-
BUILD_DIR = ../build
CODE_DIRS = core driver guest lib mem virtual #test
## END CONFIG

# build directory creation
$(shell mkdir -p $(BUILD_DIR))
$(foreach DIR,$(CODE_DIRS),$(shell mkdir -p $(BUILD_DIR)/$(DIR)))

CC = $(CROSS_COMPILE)gcc

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
$(BUILD_DIR)/$1/%_asm.o: $1/%.S $(BUILD_DIR)/structs_in_asm.h
	$(call compile,$$<,$$@)
$(BUILD_DIR)/$1/%.o: $1/%.c
	$(call compile,$$<,$$@)
endef
########################################################

default: $(BUILD_DIR)/$(NAME) tags

# generate rules for all subdirectories
$(foreach DIR, $(CODE_DIRS), $(eval $(call generate,$(DIR))))

$(BUILD_DIR)/structs_in_asm.h: helper/structs_in_asm.c
	@$(CC) $(CFLAGS) -S $^ -o - \
		| awk '($$1 == "->") { print "#define " $$2 " (" $$3 ")" }' >$@
	@echo "[i] created structs_in_asm.h"

tags: $(BUILD_DIR)/structs_in_asm.h
	@ctags -R . $^
	@echo "[i] created tags"

$(BUILD_DIR)/linkerscript.ld: helper/linkerscript.c
	@$(CC) $(INCLUDES) -P -E $^ -o $@

# WARNING: linkerscript.ld must be first dependency because -T
$(BUILD_DIR)/$(NAME): $(BUILD_DIR)/linkerscript.ld $(OBJECTS)
	@$(CC) $(CFLAGS) -T $^ -o $@
	@echo "\e[32m[+] Successfuly built $@!\e[0m"

.PHONY: clean
clean:
	@rm -f tags
	@rm -rf $(BUILD_DIR)/*
	@echo "[i] cleaned up"

.PHONY: full-rebuild
full-rebuild: clean
	cd guest/imgs && ./refresh.sh
	$(MAKE) default

-include $(DEPFILES)
