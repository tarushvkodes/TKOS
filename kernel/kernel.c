// kernel.c - Minimal kernel for TKOS
#include <stdint.h>
#include <stddef.h>
#include "idt.h"
#include "pic.h"
#include "../drivers/keyboard.h"

// VGA buffer constants
#define VGA_BUFFER 0xB8000
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_COLOR_WHITE_ON_BLACK 0x0F

// VGA color attribute byte
#define VGA_COLOR(fg, bg) ((bg << 4) | fg)

// Page directory and table entry flags
#define PAGE_PRESENT (1 << 0)
#define PAGE_READWRITE (1 << 1)

// Page tables
static uint32_t page_directory[1024] __attribute__((aligned(4096)));
static uint32_t first_page_table[1024] __attribute__((aligned(4096)));

// Current position in VGA buffer
static uint16_t* const VGA_MEMORY = (uint16_t*)VGA_BUFFER;
static size_t terminal_row = 0;
static size_t terminal_col = 0;

// Function to clear the screen
void clear_screen() {
    for (size_t i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        VGA_MEMORY[i] = VGA_COLOR_WHITE_ON_BLACK << 8 | ' ';
    }
    terminal_row = 0;
    terminal_col = 0;
}

// Function to write character at specific position
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

// Function to write string
void write_string(const char* str) {
    for (size_t i = 0; str[i] != '\0'; i++) {
        write_char(str[i]);
    }
}

void isr_handler(void) {
    write_string("Received interrupt!\n");
    // We'll expand this handler later to handle specific interrupts
}

// Function to set up basic paging
static void init_paging(void) {
    // Identity map first 16MB for initial kernel space
    for (int i = 0; i < 4096; i++) {
        first_page_table[i] = (i * 0x1000) | PAGE_PRESENT | PAGE_READWRITE;
    }

    // Map first page table
    page_directory[0] = ((uint32_t)first_page_table) | PAGE_PRESENT | PAGE_READWRITE;
    
    // Clear rest of page directory
    for (int i = 1; i < 1024; i++) {
        page_directory[i] = 0;
    }

    // Load page directory and enable paging
    __asm__ volatile (
        "mov %0, %%cr3\n"
        "mov %%cr0, %%eax\n"
        "or $0x80000000, %%eax\n"
        "mov %%eax, %%cr0"
        : : "r" (page_directory)
        : "eax"
    );
}

void kernel_main(void) {
    // Initialize terminal
    clear_screen();
    
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
    
    // Enable interrupts
    __asm__ volatile ("sti");
    
    // Write welcome message
    write_string("Welcome to TKOS!\n");
    write_string("Successfully entered protected mode.\n");
    write_string("Kernel initialized.\n");
    write_string("IDT, PIC, and keyboard initialized.\n");
    write_string("System is ready.\n");
    
    // Infinite loop with interrupts enabled
    while (1) {
        __asm__ volatile ("hlt");
    }
}

void _start(void) {
    // Set up basic VGA for debug output
    VGA_MEMORY[4] = (VGA_COLOR_WHITE_ON_BLACK << 8) | 'K';
    VGA_MEMORY[5] = (VGA_COLOR_WHITE_ON_BLACK << 8) | 'E';
    VGA_MEMORY[6] = (VGA_COLOR_WHITE_ON_BLACK << 8) | 'R';
    
    // Set up segments
    __asm__ volatile (
        "mov $0x10, %ax\n"
        "mov %ax, %ds\n"
        "mov %ax, %es\n"
        "mov %ax, %fs\n"
        "mov %ax, %gs\n"
        "mov %ax, %ss\n"
    );
    
    // Set up paging before accessing memory
    init_paging();
    
    // Initialize IDT
    if (!init_idt()) {
        VGA_MEMORY[0] = (VGA_COLOR_RED_ON_BLACK << 8) | 'E';
        while(1) __asm__ volatile ("hlt");
    }
    
    // Call kernel_main
    kernel_main();
    
    // Halt if we return from kernel_main
    while(1) {
        __asm__ volatile ("hlt");
    }
}
