#ifndef IDT_H
#define IDT_H

#include <stdint.h>

// IDT Gate Types
#define IDT_INTERRUPT_GATE 0x8E
#define IDT_TRAP_GATE 0x8F

// Structure for IDT entry
struct idt_entry {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t zero;
    uint8_t type_attr;
    uint16_t offset_high;
} __attribute__((packed));

// Structure for IDTR
struct idtr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

// Function declarations
void init_idt(void);
void idt_set_gate(uint8_t num, uint32_t handler, uint16_t selector, uint8_t flags);
void load_idt(void);

// Interrupt handler declarations
extern void isr0(void);  // Division by zero
extern void isr1(void);  // Debug
extern void isr2(void);  // Non-maskable interrupt
extern void isr3(void);  // Breakpoint
// ... Add more ISR declarations as needed

#endif