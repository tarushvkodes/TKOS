#include "isr.h"

// Array of interrupt handlers
static isr_t interrupt_handlers[256] = {0};  // Initialize all handlers to NULL

// Register an interrupt handler
void register_interrupt_handler(uint8_t n, isr_t handler) {
    if (handler != 0) {  // Validate handler
        interrupt_handlers[n] = handler;
    }
}

// Called from assembly - dispatch to the correct handler
void isr_handler(registers_t* regs) {
    if (!regs) return;  // Validate registers pointer
    
    isr_t handler = interrupt_handlers[regs->int_no];
    // If we have a handler for this interrupt, call it
    if (handler != 0) {
        handler(regs);
    } else {
        // Handle unregistered interrupt
        // TODO: Add proper error handling or logging here
    }
}