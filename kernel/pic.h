#ifndef PIC_H
#define PIC_H

#include <stdint.h>

// PIC ports
#define PIC1_COMMAND 0x20
#define PIC1_DATA    0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA    0xA1

// PIC initialization command words
#define ICW1_ICW4      0x01
#define ICW1_INIT      0x10
#define ICW4_8086      0x01

void pic_init(void);
void pic_send_eoi(uint8_t irq);

#endif