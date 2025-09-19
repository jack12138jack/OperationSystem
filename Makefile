TARGET := os-image.bin
BUILD_DIR := build
SRC_DIR := src
INCLUDE_DIR := include

NASM := nasm
CC := gcc
LD := ld
OBJCOPY := objcopy

CFLAGS := -m32 -ffreestanding -fno-pic -fno-pie -fno-stack-protector -nostdlib -nostartfiles -nodefaultlibs -Wall -Wextra -I$(INCLUDE_DIR)
LDFLAGS := -m elf_i386 -T $(SRC_DIR)/linker.ld

KERNEL_C_SOURCES := \
    $(SRC_DIR)/kernel/kernel.c \
    $(SRC_DIR)/kernel/terminal.c \
    $(SRC_DIR)/kernel/keyboard.c \
    $(SRC_DIR)/kernel/command.c \
    $(SRC_DIR)/kernel/util.c

KERNEL_ASM_SOURCES := $(SRC_DIR)/kernel/kernel_entry.asm

KERNEL_C_OBJECTS := $(patsubst $(SRC_DIR)/kernel/%.c,$(BUILD_DIR)/%.o,$(KERNEL_C_SOURCES))
KERNEL_ASM_OBJECTS := $(patsubst $(SRC_DIR)/kernel/%.asm,$(BUILD_DIR)/%.o,$(KERNEL_ASM_SOURCES))
KERNEL_OBJECTS := $(KERNEL_C_OBJECTS) $(KERNEL_ASM_OBJECTS)

.PHONY: all clean run directories

all: $(TARGET)

$(TARGET): $(BUILD_DIR)/boot.bin $(BUILD_DIR)/kernel.bin
	cat $^ > $@
	truncate -s 1M $@

$(BUILD_DIR)/boot.bin: $(SRC_DIR)/boot.asm | directories
	$(NASM) -f bin $< -o $@

$(BUILD_DIR)/kernel.bin: $(BUILD_DIR)/kernel.elf
	$(OBJCOPY) -O binary $< $@

$(BUILD_DIR)/kernel.elf: $(KERNEL_OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $(KERNEL_OBJECTS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/kernel/%.c | directories
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/kernel/%.asm | directories
	$(NASM) -f elf32 $< -o $@

directories:
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

run: $(TARGET)
	qemu-system-i386 -drive format=raw,file=$(TARGET) -serial stdio
