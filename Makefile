CC      ?= gcc
NASM    ?= yasm
LD      ?= ld
OBJCOPY ?= objcopy
QEMU    ?= qemu-system-i386

BUILD := build
OBJ   := $(BUILD)/obj
IMAGE := $(BUILD)/bbs.img
CFLAGS := -m32 -march=i686 -ffreestanding -nostdlib -fno-pie -fno-stats -Wno-unused -w -fno-builtin
ASFLAGS := -f elf32
LDFLAGS := -m elf_i386
# These files are incomplete or standalone programs, n 
CFLAGS := -m32 -march=i686 -ffreestanding -nostdlib -fno-pie -fno-stats -Wno-unused -w -fno-builtin
ASFLAGS := -f elf32
LDFLAGS := -m elf_i386

# These files are incomplete or standalone programs, not part of the default kernel.
KERNEL_C := $(shell find core drivers fs init kernel lib mpp src Clib/Stubs \
             -type f -name '*.c' \
			 ! -path 'fs/extf4.c' \
             ! -path 'kernel/ditos.c' \
             ! -path 'src/kutils/ping.c')
KERNEL_O := $(patsubst %.c,$(OBJ)/%.o,$(KERNEL_C))

BOOT_BIN   := $(BUILD)/boot.bin
KERNEL_ELF := $(BUILD)/kernel.elf
KERNEL_BIN := $(BUILD)/kernel.bin

.PHONY: all clean run

all: $(IMAGE)

$(IMAGE): $(BOOT_BIN) $(KERNEL_BIN)
	mkdir -p $(dir $@)
	dd if=/dev/zero of=$@ bs=512 count=66 status=none
	dd if=$(BOOT_BIN) of=$@ bs=512 count=1 conv=notrunc status=none
	dd if=$(KERNEL_BIN) of=$@ bs=512 seek=2 conv=notrunc status=none

$(BOOT_BIN): bootloader/bootloader.asm bootloader/disk.asm bootloader/kernel_entry.asm bootloader/gdt.asm
	mkdir -p $(dir $@)
	$(NASM) -f bin -o $@ bootloader/bootloader.asm
	@test "$$(wc -c < $@)" -eq 512 || { echo "boot sector must be exactly 512 bytes"; exit 1; }

$(KERNEL_BIN): $(KERNEL_ELF)
	$(OBJCOPY) -O binary $< $@
	@test "$$(wc -c < $@)" -le $$((64 * 512)) || { echo "kernel exceeds loader capacity of 64 sectors"; exit 1; }

$(KERNEL_ELF): kernel.ld $(KERNEL_O)
	mkdir -p $(dir $@)
	$(LD) $(LDFLAGS) -T kernel.ld -e BBS_kernel -o $@ $(KERNEL_O)

$(OBJ)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I. -IClib -IClib/Xlibary -Ikernel/include -Iinit/include -Ilib/include -Isrc/kshell -c $< -o $@

run: $(IMAGE)
	$(QEMU) -drive file=$(IMAGE),format=raw,if=ide -nic none -accel tcg -display curses

clean:
	rm -rf $(BUILD)