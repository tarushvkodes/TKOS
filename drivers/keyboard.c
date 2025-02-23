#include "keyboard.h"
#include "../kernel/port_io.h"
#include "../kernel/isr.h"
#include <stdbool.h>

// Basic US keyboard scancode map (set 1)
static const char scancode_map[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' '
};

#define KEYBOARD_DATA_PORT    0x60
#define KEYBOARD_STATUS_PORT  0x64
#define KEYBOARD_COMMAND_PORT 0x64

#define KEYBOARD_ACK         0xFA
#define KEYBOARD_RESEND      0xFE

static void keyboard_callback(registers_t *regs) {
    uint8_t scancode = inb(KEYBOARD_DATA_PORT);
    // Handle the keyboard input here
    // For now, we just acknowledge we received it
    (void)scancode; // Suppress unused variable warning
    (void)regs;     // Suppress unused variable warning
}

static bool keyboard_wait_input(void) {
    int timeout = 100000;
    while (--timeout) {
        if (!(inb(KEYBOARD_STATUS_PORT) & 0x02))
            return true;
    }
    return false;
}

static bool keyboard_wait_output(void) {
    int timeout = 100000;
    while (--timeout) {
        if (inb(KEYBOARD_STATUS_PORT) & 0x01)
            return true;
    }
    return false;
}

bool init_keyboard(void) {
    // Wait for keyboard to be ready
    if (!keyboard_wait_input())
        return false;

    // Reset keyboard
    outb(KEYBOARD_COMMAND_PORT, 0xFF);
    
    // Wait for acknowledgment
    if (!keyboard_wait_output())
        return false;
    
    uint8_t response = inb(KEYBOARD_DATA_PORT);
    if (response != KEYBOARD_ACK && response != KEYBOARD_RESEND)
        return false;

    // Register our keyboard handler
    register_interrupt_handler(33, keyboard_callback); // IRQ1 -> INT 33

    return true;
}

char scancode_to_ascii(uint8_t scancode) {
    // Only handle "key press" events (not key release)
    if (scancode & 0x80) {
        return 0;
    }
    return scancode_map[scancode];
}