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

// Removed duplicate ISR stub definitions and inline implementations from isr.h. Retained only declarations.
#ifndef ISR_H
#define ISR_H

#include <stdint.h>

// Structure representing CPU registers pushed by an ISR
typedef struct {
    uint32_t ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags, useresp, ss;
} registers_t;

// Exception messages for debugging (defined in isr_impl or similar)
extern const char *exception_messages[];

// ISR handler function which gets called by the common ISR stub
void isr_handler(registers_t *regs);

// Registers a custom interrupt handler for interrupt number n
void register_interrupt_handler(uint8_t n, void (*handler)(registers_t*));

// Declarations for ISR stubs 0-31 (implemented in isr.asm)
extern void isr0(void);
extern void isr1(void);
extern void isr2(void);
extern void isr3(void);
extern void isr4(void);
extern void isr5(void);
extern void isr6(void);
extern void isr7(void);
extern void isr8(void);
extern void isr9(void);
extern void isr10(void);
extern void isr11(void);
extern void isr12(void);
extern void isr13(void);
extern void isr14(void);
extern void isr15(void);
extern void isr16(void);
extern void isr17(void);
extern void isr18(void);
extern void isr19(void);
extern void isr20(void);
extern void isr21(void);
extern void isr22(void);
extern void isr23(void);
extern void isr24(void);
extern void isr25(void);
extern void isr26(void);
extern void isr27(void);
extern void isr28(void);
extern void isr29(void);
extern void isr30(void);
extern void isr31(void);

#endif // ISR_H