#include "pic.h"
#include "port_io.h"
#include <stdbool.h>

// Initialization command words
#define ICW1_ICW4       0x01    /* ICW4 needed */
#define ICW1_SINGLE     0x02    /* Single (cascade) mode */
#define ICW1_INTERVAL4  0x04    /* Call address interval 4 (8) */
#define ICW1_LEVEL      0x08    /* Level triggered (edge) mode */
#define ICW1_INIT       0x10    /* Initialization - required! */
 
#define ICW4_8086       0x01    /* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO       0x02    /* Auto (normal) EOI */
#define ICW4_BUF_SLAVE  0x08    /* Buffered mode/slave */
#define ICW4_BUF_MASTER 0x0C    /* Buffered mode/master */
#define ICW4_SFNM       0x10    /* Special fully nested (not) */

// OCW3 commands
#define PIC_READ_IRR    0x0a
#define PIC_READ_ISR    0x0b

static uint16_t pic_get_irr(void) {
    outb(PIC1_COMMAND, PIC_READ_IRR);
    outb(PIC2_COMMAND, PIC_READ_IRR);
    return (inb(PIC2_COMMAND) << 8) | inb(PIC1_COMMAND);
}

static uint16_t pic_get_isr(void) {
    outb(PIC1_COMMAND, PIC_READ_ISR);
    outb(PIC2_COMMAND, PIC_READ_ISR);
    return (inb(PIC2_COMMAND) << 8) | inb(PIC1_COMMAND);
}

static void io_wait(void) {
    outb(0x80, 0); // Write to an unused port
}

bool pic_init(void) {
    uint8_t a1, a2;
 
    // Save masks
    a1 = inb(PIC1_DATA);
    a2 = inb(PIC2_DATA);
 
    // Start initialization sequence
    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();
    
    // Remap IRQ0-7 to 32-39 and IRQ8-15 to 40-47
    outb(PIC1_DATA, 0x20);  // Master PIC vector offset (32)
    io_wait();
    outb(PIC2_DATA, 0x28);  // Slave PIC vector offset (40)
    io_wait();
    
    // Setup cascading
    outb(PIC1_DATA, 4);     // Tell Master about Slave at IRQ2
    io_wait();
    outb(PIC2_DATA, 2);     // Tell Slave its cascade identity
    io_wait();
 
    // Set 8086/88 mode
    outb(PIC1_DATA, ICW4_8086);
    io_wait();
    outb(PIC2_DATA, ICW4_8086);
    io_wait();
 
    // Restore saved masks
    outb(PIC1_DATA, a1);
    outb(PIC2_DATA, a2);
    
    return true;
}

void pic_send_eoi(unsigned char irq) {
    // Ensure IRQ number is valid
    if (irq > 15) return;
    
    // Send EOI to slave PIC if this is a slave interrupt
    if (irq >= 8) {
        outb(PIC2_COMMAND, 0x20);
    }
    
    // Always send EOI to master PIC
    outb(PIC1_COMMAND, 0x20);
}

void pic_set_mask(unsigned char irq) {
    uint16_t port;
    uint8_t value;
    
    if (irq < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        irq -= 8;
    }
    value = inb(port) | (1 << irq);
    outb(port, value);
}

void pic_clear_mask(unsigned char irq) {
    uint16_t port;
    uint8_t value;
    
    if (irq < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        irq -= 8;
    }
    value = inb(port) & ~(1 << irq);
    outb(port, value);
}

void pic_disable(void) {
    // Mask all interrupts
    outb(PIC1_DATA, 0xFF);
    outb(PIC2_DATA, 0xFF);
}