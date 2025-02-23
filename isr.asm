; isr.asm - Interrupt Service Routines
[BITS 32]
section .text

; Constants
KERNEL_DS equ 0x10     ; Kernel data segment selector

; External C function
extern isr_handler

; Export our ASM routines
global isr_common_stub
global load_idt
global isr0, isr1, isr2, isr3, isr4, isr5, isr6, isr7, isr8, isr9
global isr10, isr11, isr12, isr13, isr14, isr15, isr16, isr17, isr18, isr19

section .isr_text
align 4

; Load IDT
load_idt:
    extern idt_ptr
    lidt [idt_ptr]
    sti                     ; Enable interrupts after loading IDT
    ret

; Common ISR stub that calls our C handler
isr_common_stub:
    pusha                   ; Push all registers
    mov ax, ds             ; Save data segment
    push eax
    
    mov ax, KERNEL_DS      ; Load kernel data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    push esp               ; Push pointer to registers_t struct as argument
    call isr_handler       ; Call C handler
    add esp, 4            ; Clean up pushed argument
    
    pop eax               ; Restore data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    popa                  ; Restore registers
    add esp, 8           ; Clean up error code and ISR number
    iret                 ; Return from interrupt

; CPU Exception handlers
%macro ISR_NOERRCODE 1
align 4
isr%1:
    cli                  ; Disable interrupts
    push dword 0        ; Push dummy error code
    push dword %1       ; Push interrupt number
    jmp isr_common_stub ; Go to common handler
%endmacro

%macro ISR_ERRCODE 1
align 4
isr%1:
    cli                 ; Disable interrupts
    push dword %1      ; Push interrupt number
    jmp isr_common_stub ; Go to common handler
%endmacro

; Define ISRs with correct error code handling
ISR_NOERRCODE 0   ; Division by zero
ISR_NOERRCODE 1   ; Debug
ISR_NOERRCODE 2   ; Non-maskable interrupt
ISR_NOERRCODE 3   ; Breakpoint
ISR_NOERRCODE 4   ; Overflow
ISR_NOERRCODE 5   ; Bound range exceeded
ISR_NOERRCODE 6   ; Invalid opcode
ISR_NOERRCODE 7   ; Device not available
ISR_ERRCODE   8   ; Double fault
ISR_ERRCODE   9   ; Coprocessor segment overrun (now generates error code)
ISR_ERRCODE   10  ; Invalid TSS
ISR_ERRCODE   11  ; Segment not present
ISR_ERRCODE   12  ; Stack-segment fault
ISR_ERRCODE   13  ; General protection fault
ISR_ERRCODE   14  ; Page fault
ISR_NOERRCODE 15  ; Reserved
ISR_NOERRCODE 16  ; x87 floating-point exception
ISR_ERRCODE   17  ; Alignment check (now generates error code)
ISR_NOERRCODE 18  ; Machine check
ISR_NOERRCODE 19  ; SIMD floating-point exception