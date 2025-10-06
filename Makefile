BUILD_DIR := build
BOOT_SRC := boot/boot.S
BOOT_OBJ := $(BUILD_DIR)/boot.o
BOOT_ELF := $(BUILD_DIR)/boot.elf
BOOT_BIN := $(BUILD_DIR)/boot.bin

KERNEL_C_SOURCES := $(wildcard kernel/*.c)
KERNEL_S_SOURCES := $(wildcard kernel/*.S)
KERNEL_OBJECTS := $(patsubst kernel/%.c,$(BUILD_DIR)/kernel/%.o,$(KERNEL_C_SOURCES)) \
                   $(patsubst kernel/%.S,$(BUILD_DIR)/kernel/%.o,$(KERNEL_S_SOURCES))

KERNEL_ELF := $(BUILD_DIR)/kernel.elf
KERNEL_BIN := $(BUILD_DIR)/kernel.bin
LINKER_SCRIPT := linker.ld
OS_IMAGE := $(BUILD_DIR)/os-image.bin

CFLAGS := -std=gnu11 -m32 -ffreestanding -fno-pic -fno-pie -fno-stack-protector -Wall -Wextra -O2 -Iinclude
ASFLAGS := -m32 -ffreestanding
LDFLAGS := -melf_i386 -nostdlib -T $(LINKER_SCRIPT)

.PHONY: all clean run

all: $(OS_IMAGE)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/kernel:
	mkdir -p $(BUILD_DIR)/kernel

$(BOOT_OBJ): $(BOOT_SRC) | $(BUILD_DIR)
	gcc -m32 -c $< -o $@

$(BOOT_ELF): $(BOOT_OBJ)
	ld -m elf_i386 -Ttext 0x7C00 -nostdlib $< -o $@

$(BOOT_BIN): $(BOOT_ELF)
	objcopy -O binary $< $@

$(BUILD_DIR)/kernel/%.o: kernel/%.c | $(BUILD_DIR)/kernel
	gcc $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/kernel/%.o: kernel/%.S | $(BUILD_DIR)/kernel
	gcc -m32 -c $< -o $@

$(KERNEL_ELF): $(KERNEL_OBJECTS) $(LINKER_SCRIPT)
	ld $(LDFLAGS) -o $@ $(KERNEL_OBJECTS)

$(KERNEL_BIN): $(KERNEL_ELF)
	objcopy -O binary $< $@

$(OS_IMAGE): $(BOOT_BIN) $(KERNEL_BIN) scripts/build_image.py
	python3 scripts/build_image.py $(BOOT_ELF) $(BOOT_BIN) $(KERNEL_BIN) $@

run: $(OS_IMAGE)
	qemu-system-i386 -drive format=raw,file=$(OS_IMAGE)

clean:
	rm -rf $(BUILD_DIR)
