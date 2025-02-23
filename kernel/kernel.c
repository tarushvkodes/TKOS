// kernel.c - Minimal kernel for TKOS
#include <stdint.h>

// VGA buffer constants
#define VGA_BUFFER 0xB8000
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_COLOR_WHITE_ON_BLACK 0x0F

// VGA color attribute byte
#define VGA_COLOR(fg, bg) ((bg << 4) | fg)

// Current position in VGA buffer
static volatile uint16_t* const VGA_MEMORY = (volatile uint16_t*)VGA_BUFFER;
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

void kernel_main(void) {
    // Initialize terminal
    clear_screen();
    
    // Write welcome message
    write_string("Welcome to TKOS!\n");
    write_string("Successfully entered protected mode.\n");
    write_string("Kernel initialized.\n");

    // Halt CPU but leave interrupts enabled for now
    while (1) {
        __asm__ volatile ("hlt");
    }
}

void _start(void) {
    // Initialize segments
    __asm__ volatile (
        "mov $0x10, %ax\n\t"
        "mov %ax, %ds\n\t"
        "mov %ax, %es\n\t"
        "mov %ax, %fs\n\t"
        "mov %ax, %gs\n\t"
        "mov %ax, %ss\n\t"
        ::: "ax"
    );

    // Call kernel
    kernel_main();
}
