#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>
#include <stdbool.h>

// Keyboard controller ports
#define KEYBOARD_DATA_PORT    0x60
#define KEYBOARD_STATUS_PORT  0x64
#define KEYBOARD_COMMAND_PORT 0x64

// Keyboard-related IRQ
#define KEYBOARD_IRQ 1

// Function declarations
bool init_keyboard(void);
void keyboard_handler(void);

// Basic US keyboard scancode conversion
char scancode_to_ascii(uint8_t scancode);

#endif