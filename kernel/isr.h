// Reset ISR stub
#define ISR_STUB(isr_number) \
    void isr##isr_number(void) { \
        __asm__ volatile ( \
            "cli\n" \
            "pushl $0\n" \
            "pushl $" #isr_number "\n" \
            "jmp isr_common_stub" \
        ); \
    }

// Common ISR stub that calls our C handler
__attribute__((naked)) void isr_common_stub(void) {
    __asm__ volatile (
        "pusha\n"                // Push all registers
        "mov %ds, %ax\n"         // Save data segment
        "push %eax\n"
        "mov $0x10, %ax\n"       // Load kernel data segment
        "mov %ax, %ds\n"
        "mov %ax, %es\n"
        "mov %ax, %fs\n"
        "mov %ax, %gs\n"
        "call isr_handler\n"      // Call C handler
        "pop %eax\n"             // Restore data segment
        "mov %ax, %ds\n"
        "mov %ax, %es\n"
        "mov %ax, %fs\n"
        "mov %ax, %gs\n"
        "popa\n"                 // Restore registers
        "add $8, %esp\n"         // Clean up stack
        "iret\n"                 // Return from interrupt
    );
}

// Generate first 32 ISRs (CPU exceptions)
ISR_STUB(0)   // Division by zero
ISR_STUB(1)   // Debug
ISR_STUB(2)   // Non-maskable interrupt
ISR_STUB(3)   // Breakpoint
ISR_STUB(4)   // Overflow
ISR_STUB(5)   // Bound range exceeded
ISR_STUB(6)   // Invalid opcode
ISR_STUB(7)   // Device not available
ISR_STUB(8)   // Double fault
ISR_STUB(9)   // Coprocessor segment overrun
ISR_STUB(10)  // Invalid TSS
ISR_STUB(11)  // Segment not present
ISR_STUB(12)  // Stack-segment fault
ISR_STUB(13)  // General protection fault
ISR_STUB(14)  // Page fault
ISR_STUB(15)  // Reserved
ISR_STUB(16)  // x87 FPU error
ISR_STUB(17)  // Alignment check
ISR_STUB(18)  // Machine check
ISR_STUB(19)  // SIMD floating-point
// ... etc up to 31

// External declarations of our ISR handlers
#ifndef ISR_H
#define ISR_H

#include <stdint.h>

// Interrupt handler function type
typedef struct {
    uint32_t ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags, useresp, ss;
} registers_t;

// Exception messages for better debugging
static const char *exception_messages[] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Overflow",
    "Bound Range Exceeded",
    "Invalid Opcode",
    "Device Not Available",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Invalid TSS",
    "Segment Not Present",
    "Stack-Segment Fault",
    "General Protection Fault",
    "Page Fault",
    "Reserved",
    "x87 FPU Error",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point"
};

// Function declarations
void isr_handler(registers_t *regs);
typedef void (*isr_t)(registers_t*);
void register_interrupt_handler(uint8_t n, isr_t handler);

// Modified ISR_STUB macro with static inline to ensure internal linkage
#undef ISR_STUB
#define ISR_STUB(isr_number) \
    static inline void isr##isr_number(void) { \
        /* ISR stub code for interrupt isr_number */ \
    }

// Modified isr_common_stub definition to be static inline
#undef isr_common_stub
static inline __attribute__((naked)) void isr_common_stub(void) {
    /* Common ISR handler code */
}

// ISR stubs
#define ISR_STUB(num) extern void isr##num(void)

// Define ISR stubs 0-31
ISR_STUB(0);  ISR_STUB(1);  ISR_STUB(2);  ISR_STUB(3);
ISR_STUB(4);  ISR_STUB(5);  ISR_STUB(6);  ISR_STUB(7);
ISR_STUB(8);  ISR_STUB(9);  ISR_STUB(10); ISR_STUB(11);
ISR_STUB(12); ISR_STUB(13); ISR_STUB(14); ISR_STUB(15);
ISR_STUB(16); ISR_STUB(17); ISR_STUB(18); ISR_STUB(19);
ISR_STUB(20); ISR_STUB(21); ISR_STUB(22); ISR_STUB(23);
ISR_STUB(24); ISR_STUB(25); ISR_STUB(26); ISR_STUB(27);
ISR_STUB(28); ISR_STUB(29); ISR_STUB(30); ISR_STUB(31);

#endif // ISR_H