#ifndef PIC_H
#define PIC_H

#include <stdbool.h>
#include <stdint.h>

// PIC I/O ports
#define PIC1_COMMAND 0x20
#define PIC1_DATA    0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA    0xA1

// Function declarations
bool pic_init(void);
void pic_send_eoi(unsigned char irq);
void pic_set_mask(unsigned char irq);    // Mask (disable) an IRQ line
void pic_clear_mask(unsigned char irq);  // Unmask (enable) an IRQ line
void pic_disable(void);

#endif /* PIC_H */