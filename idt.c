#include "idt.h"
#include "isr.h"

struct idt_entry idt[256];
struct idt_ptr idt_ptr;

static void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    if (base == 0) return;  // Prevent null handler registration
    
    idt[num].base_lo = base & 0xFFFF;
    idt[num].base_hi = (base >> 16) & 0xFFFF;
    idt[num].sel = sel;
    idt[num].always0 = 0;
    idt[num].flags = flags;
}

bool init_idt(void) {
    // Set up IDT pointer
    idt_ptr.limit = (sizeof(struct idt_entry) * 256) - 1;
    idt_ptr.base = (uint32_t)&idt;

    // Clear IDT
    for(int i = 0; i < 256; i++) {
        idt_set_gate(i, 0, 0, 0);
    }

    // CPU Exception handlers - all use interrupt gates
    idt_set_gate(0, (uint32_t)isr0, 0x08, IDT_INTERRUPT_GATE);   // Division by zero
    idt_set_gate(1, (uint32_t)isr1, 0x08, IDT_INTERRUPT_GATE);   // Debug
    idt_set_gate(2, (uint32_t)isr2, 0x08, IDT_INTERRUPT_GATE);   // NMI
    idt_set_gate(3, (uint32_t)isr3, 0x08, IDT_TRAP_GATE);        // Breakpoint uses trap gate
    idt_set_gate(4, (uint32_t)isr4, 0x08, IDT_INTERRUPT_GATE);   // Overflow
    idt_set_gate(5, (uint32_t)isr5, 0x08, IDT_INTERRUPT_GATE);   // Bound range
    idt_set_gate(6, (uint32_t)isr6, 0x08, IDT_INTERRUPT_GATE);   // Invalid opcode
    idt_set_gate(7, (uint32_t)isr7, 0x08, IDT_INTERRUPT_GATE);   // Device not available
    idt_set_gate(8, (uint32_t)isr8, 0x08, IDT_INTERRUPT_GATE);   // Double fault
    idt_set_gate(9, (uint32_t)isr9, 0x08, IDT_INTERRUPT_GATE);   // Coprocessor
    idt_set_gate(10, (uint32_t)isr10, 0x08, IDT_INTERRUPT_GATE); // Invalid TSS
    idt_set_gate(11, (uint32_t)isr11, 0x08, IDT_INTERRUPT_GATE); // Segment not present
    idt_set_gate(12, (uint32_t)isr12, 0x08, IDT_INTERRUPT_GATE); // Stack fault
    idt_set_gate(13, (uint32_t)isr13, 0x08, IDT_INTERRUPT_GATE); // Protection fault
    idt_set_gate(14, (uint32_t)isr14, 0x08, IDT_INTERRUPT_GATE); // Page fault
    idt_set_gate(15, (uint32_t)isr15, 0x08, IDT_INTERRUPT_GATE); // Reserved
    idt_set_gate(16, (uint32_t)isr16, 0x08, IDT_INTERRUPT_GATE); // FPU error
    idt_set_gate(17, (uint32_t)isr17, 0x08, IDT_INTERRUPT_GATE); // Alignment check
    idt_set_gate(18, (uint32_t)isr18, 0x08, IDT_INTERRUPT_GATE); // Machine check
    idt_set_gate(19, (uint32_t)isr19, 0x08, IDT_INTERRUPT_GATE); // SIMD error

    // Load IDT
    load_idt();
    
    return true;
}