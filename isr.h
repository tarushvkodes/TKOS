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

#ifndef ISR_H
#define ISR_H

#include <stdint.h>

// Define the register structure for interrupt handlers
typedef struct {
    uint32_t ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags, useresp, ss;
} registers_t;

// Function pointer type for interrupt handlers
typedef void (*isr_t)(registers_t*);

// Handler registration function - implemented in isr.c
void register_interrupt_handler(uint8_t n, isr_t handler);

// Handler called from assembly - implemented in isr.c
void isr_handler(registers_t* regs);

// Common ISR stub - implemented in isr.asm
void __attribute__((weak)) isr_common_stub(void);

// Assembly ISR stubs - implemented in isr.asm
void __attribute__((weak)) isr0(void);
void __attribute__((weak)) isr1(void);
void __attribute__((weak)) isr2(void);
void __attribute__((weak)) isr3(void);
void __attribute__((weak)) isr4(void);
void __attribute__((weak)) isr5(void);
void __attribute__((weak)) isr6(void);
void __attribute__((weak)) isr7(void);
void __attribute__((weak)) isr8(void);
void __attribute__((weak)) isr9(void);
void __attribute__((weak)) isr10(void);
void __attribute__((weak)) isr11(void);
void __attribute__((weak)) isr12(void);
void __attribute__((weak)) isr13(void);
void __attribute__((weak)) isr14(void);
void __attribute__((weak)) isr15(void);
void __attribute__((weak)) isr16(void);
void __attribute__((weak)) isr17(void);
void __attribute__((weak)) isr18(void);
void __attribute__((weak)) isr19(void);

#endif