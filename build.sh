#!/bin/bash

# Set cross-compiler commands and flags
CC=x86_64-elf-gcc
LD=x86_64-elf-ld
CFLAGS="-m32 -ffreestanding -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -I. -I./libs -I./kernel -I./drivers"
LDFLAGS="-melf_i386 -T linker.ld"

# Create build directory
mkdir -p build

# Compile assembly files
nasm -f elf32 kernel/isr.asm -o build/isr.o
nasm -f bin bootloader/bootloader.asm -o build/bootloader.bin

# Compile C files
$CC $CFLAGS -c kernel/kernel.c -o build/kernel.o
$CC $CFLAGS -c kernel/idt.c -o build/idt.o
$CC $CFLAGS -c kernel/pic.c -o build/pic.o
$CC $CFLAGS -c drivers/keyboard.c -o build/keyboard.o

# Link kernel
$LD $LDFLAGS -o build/kernel.bin build/kernel.o build/idt.o build/isr.o build/pic.o build/keyboard.o

# Create disk image
dd if=/dev/zero of=build/bootloader.img bs=512 count=2880
dd if=build/bootloader.bin of=build/bootloader.img conv=notrunc
dd if=build/kernel.bin of=build/bootloader.img seek=1 conv=notrunc

echo "Build complete! Output files are in the build directory."