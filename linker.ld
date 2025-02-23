ENTRY(_start)
OUTPUT_FORMAT(elf32-i386)
OUTPUT_ARCH(i386)

SECTIONS {
    . = 0x1000;    /* Match KERNEL_OFFSET from bootloader */
    
    .isr_text BLOCK(4K) : ALIGN(4K) {
        *(.isr_text)
    }

    .text BLOCK(4K) : ALIGN(4K) {
        *(.text)
    }

    .rodata BLOCK(4K) : ALIGN(4K) {
        *(.rodata)
    }

    .data BLOCK(4K) : ALIGN(4K) {
        *(.data)
    }

    .bss BLOCK(4K) : ALIGN(4K) {
        *(COMMON)
        *(.bss)
    }
}