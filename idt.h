#ifndef IDT_H
#define IDT_H

#include <stdint.h>
#include <stdbool.h>

// IDT gate types
#define IDT_INTERRUPT_GATE 0x8E    // Present(1)|Ring0(00)|Type(1110)
#define IDT_TRAP_GATE     0x8F    // Present(1)|Ring0(00)|Type(1111)

// IDT entry structure
struct idt_entry {
    uint16_t base_lo;
    uint16_t sel;
    uint8_t always0;
    uint8_t flags;
    uint16_t base_hi;
} __attribute__((packed));

// IDT pointer structure
struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

// Function declarations
bool init_idt(void);
extern void load_idt(void);

#endif