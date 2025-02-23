#!/bin/bash

# Set cross-compiler commands and flags
CC=x86_64-elf-gcc
LD=x86_64-elf-ld
NASM=nasm
CFLAGS="-m32 -ffreestanding -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -fno-common -I. -I./libs -I./kernel -I./drivers"
LDFLAGS="-melf_i386 -T linker.ld"

# Create build directory
mkdir -p build

# Clean old object files to avoid stale artifacts
rm -f build/*.o build/*.bin build/*.img

# Compile bootloader
$NASM -f bin bootloader/bootloader.asm -o build/bootloader.bin

# Compile assembly files
$NASM -f elf32 kernel/isr.asm -o build/isr_asm.o

# Compile C source files
$CC $CFLAGS -c kernel/isr.c -o build/isr.o
$CC $CFLAGS -c kernel/idt.c -o build/idt.o
$CC $CFLAGS -c kernel/pic.c -o build/pic.o
$CC $CFLAGS -c kernel/memory.c -o build/memory.o
$CC $CFLAGS -c kernel/kernel.c -o build/kernel.o
$CC $CFLAGS -c drivers/keyboard.c -o build/keyboard.o

# Link kernel - crucial to link isr_asm.o first to resolve ISR symbols
$LD $LDFLAGS -o build/kernel.bin \
    build/isr_asm.o \
    build/isr.o \
    build/idt.o \
    build/pic.o \
    build/memory.o \
    build/kernel.o \
    build/keyboard.o

# Create disk image
dd if=/dev/zero of=build/bootloader.img bs=512 count=2880
dd if=build/bootloader.bin of=build/bootloader.img conv=notrunc bs=512
dd if=build/kernel.bin of=build/bootloader.img conv=notrunc bs=512 seek=1

echo "Build complete! Output files are in the build directory."