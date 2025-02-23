#include "keyboard.h"
#include "../kernel/port_io.h"
#include "../kernel/isr.h"
#include <stdbool.h>

// Keyboard controller commands
#define KEYBOARD_RESET       0xFF
#define KEYBOARD_ENABLE      0xF4
#define KEYBOARD_LED_CMD     0xED
#define KEYBOARD_ACK        0xFA
#define KEYBOARD_RESEND     0xFE
#define KEYBOARD_SELF_TEST  0xAA
#define KEYBOARD_TEST_OK    0x55

// Basic US keyboard scancode map (set 1)
static const char scancode_map[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' '
};

static volatile bool keyboard_initialized = false;

static void keyboard_callback(registers_t *regs) {
    if (!regs) return;
    
    // Read scancode and status
    uint8_t status = inb(KEYBOARD_STATUS_PORT);
    if (!(status & 0x01)) return;  // No data available
    
    uint8_t scancode = inb(KEYBOARD_DATA_PORT);
    
    // Process the scancode
    if (!(scancode & 0x80)) {  // Key press event
        char ascii = scancode_to_ascii(scancode);
        if (ascii) {
            // Handle the character (could add buffer here)
        }
    }
}

static bool keyboard_wait_input(void) {
    for (int timeout = 100000; timeout > 0; timeout--) {
        if (!(inb(KEYBOARD_STATUS_PORT) & 0x02))
            return true;
    }
    return false;
}

static bool keyboard_wait_output(void) {
    for (int timeout = 100000; timeout > 0; timeout--) {
        if (inb(KEYBOARD_STATUS_PORT) & 0x01)
            return true;
    }
    return false;
}

static bool keyboard_send_command(uint8_t command) {
    if (!keyboard_wait_input()) return false;
    outb(KEYBOARD_COMMAND_PORT, command);
    
    if (!keyboard_wait_output()) return false;
    return (inb(KEYBOARD_DATA_PORT) == KEYBOARD_ACK);
}

bool init_keyboard(void) {
    if (keyboard_initialized) return true;
    
    // Reset the keyboard
    if (!keyboard_wait_input()) return false;
    outb(KEYBOARD_COMMAND_PORT, KEYBOARD_RESET);
    
    // Wait for self-test completion
    if (!keyboard_wait_output()) return false;
    if (inb(KEYBOARD_DATA_PORT) != KEYBOARD_TEST_OK) return false;
    
    // Enable scanning
    if (!keyboard_send_command(KEYBOARD_ENABLE)) return false;
    
    // Register our keyboard handler (IRQ1 -> INT 33)
    register_interrupt_handler(33, keyboard_callback);
    
    keyboard_initialized = true;
    return true;
}

char scancode_to_ascii(uint8_t scancode) {
    if (scancode >= 128) return 0;  // Handle only press events
    return scancode_map[scancode];
}