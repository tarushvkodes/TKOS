// kernel.c - Minimal kernel for TKOS
#include <stdint.h>
#include <stddef.h>
#include "idt.h"
#include "pic.h"
#include "isr.h"
#include "memory.h"
#include "../drivers/keyboard.h"

// VGA buffer constants
#define VGA_BUFFER 0xB8000
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

// VGA colors
#define VGA_BLACK        0x0
#define VGA_BLUE         0x1
#define VGA_GREEN        0x2
#define VGA_CYAN         0x3
#define VGA_RED          0x4
#define VGA_MAGENTA      0x5
#define VGA_BROWN        0x6
#define VGA_LIGHT_GREY   0x7
#define VGA_DARK_GREY    0x8
#define VGA_LIGHT_BLUE   0x9
#define VGA_LIGHT_GREEN  0xA
#define VGA_LIGHT_CYAN   0xB
#define VGA_LIGHT_RED    0xC
#define VGA_LIGHT_MAGENTA 0xD
#define VGA_LIGHT_BROWN  0xE
#define VGA_WHITE        0xF

#define VGA_COLOR_WHITE_ON_BLACK 0x0F
#define VGA_COLOR_RED_ON_BLACK   0x04

// VGA color attribute byte
#define VGA_COLOR(fg, bg) ((bg << 4) | fg)

// Current position in VGA buffer
static uint16_t* const VGA_MEMORY = (uint16_t*)VGA_BUFFER;
static size_t terminal_row = 0;
static size_t terminal_col = 0;

// Function prototypes
void clear_screen(void);
void write_char(char c);
void write_string(const char* str);

// Function implementations
void clear_screen(void) {
    for (size_t i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        VGA_MEMORY[i] = VGA_COLOR_WHITE_ON_BLACK << 8 | ' ';
    }
    terminal_row = 0;
    terminal_col = 0;
}

void write_char(char c) {
    if (c == '\n') {
        terminal_col = 0;
        terminal_row++;
        if (terminal_row >= VGA_HEIGHT) {
            terminal_row = 0;
        }
        return;
    }

    const size_t index = terminal_row * VGA_WIDTH + terminal_col;
    VGA_MEMORY[index] = VGA_COLOR_WHITE_ON_BLACK << 8 | c;
    
    terminal_col++;
    if (terminal_col >= VGA_WIDTH) {
        terminal_col = 0;
        terminal_row++;
        if (terminal_row >= VGA_HEIGHT) {
            terminal_row = 0;
        }
    }
}

void write_string(const char* str) {
    for (size_t i = 0; str[i] != '\0'; i++) {
        write_char(str[i]);
    }
}

void kernel_main(void) {
    // Initialize terminal
    clear_screen();
    
    // Initialize IDT
    if (!init_idt()) {
        write_string("Error: IDT initialization failed\n");
        return;
    }

    // Initialize and remap PIC
    if (!pic_init()) {
        write_string("Error: PIC initialization failed\n");
        return;
    }
    
    // Initialize keyboard
    if (!init_keyboard()) {
        write_string("Error: Keyboard initialization failed\n");
        return;
    }

    // Initialize memory system - start heap at 1MB mark with 4MB size
    init_memory(0x100000, 0x400000);
    
    // Enable interrupts
    __asm__ volatile ("sti");
    
    // Write welcome message
    write_string("Welcome to TKOS!\n");
    write_string("Successfully entered protected mode.\n");
    write_string("Kernel initialized.\n");
    write_string("IDT, PIC, keyboard, and memory management initialized.\n");
    write_string("System is ready.\n");
    
    // Test memory allocation
    void* test_alloc = kmalloc(1024);
    if (test_alloc != NULL) {
        write_string("Memory allocation test successful.\n");
    } else {
        write_string("Memory allocation test failed.\n");
    }
    
    // Infinite loop with interrupts enabled
    while (1) {
        __asm__ volatile ("hlt");
    }
}

void _start(void) {
    // Set up basic VGA for debug output
    VGA_MEMORY[0] = (VGA_COLOR_WHITE_ON_BLACK << 8) | 'K';
    
    // Set up segments
    __asm__ volatile (
        "mov $0x10, %ax\n"
        "mov %ax, %ds\n"
        "mov %ax, %es\n"
        "mov %ax, %fs\n"
        "mov %ax, %gs\n"
        "mov %ax, %ss\n"
    );
    
    // Call kernel_main
    kernel_main();
    
    // Halt if we return from kernel_main
    while(1) {
        __asm__ volatile ("hlt");
    }
}
