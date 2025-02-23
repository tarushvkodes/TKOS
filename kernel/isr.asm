; isr.asm - Interrupt Service Routines
[BITS 32]

global load_idt
global isr0
global isr1
global isr2
global isr3
global isr4
global isr5
global isr6
global isr7
global isr8
global isr9
global isr10
global isr11
global isr12
global isr13
global isr14
global isr15
global isr16
global isr17
global isr18
global isr19

extern isr_handler

; Load IDT
load_idt:
    extern idt_ptr
    lidt [idt_ptr]
    ret

; Common ISR stub that calls our C handler
isr_common_stub:
    pusha                    ; Push all registers
    mov ax, ds              ; Save data segment
    push eax
    
    mov ax, 0x10            ; Load kernel data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    call isr_handler        ; Call C handler
    
    pop eax                 ; Restore data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    popa                    ; Restore registers
    add esp, 8             ; Clean up error code and ISR number
    iret                   ; Return from interrupt

; CPU Exception handlers
%macro ISR_NOERRCODE 1
isr%1:
    cli                    ; Disable interrupts
    push dword 0          ; Push dummy error code
    push dword %1         ; Push interrupt number
    jmp isr_common_stub   ; Go to common handler
%endmacro

%macro ISR_ERRCODE 1
isr%1:
    cli                    ; Disable interrupts
    push dword %1         ; Push interrupt number
    jmp isr_common_stub   ; Go to common handler
%endmacro

; Define ISRs
ISR_NOERRCODE 0   ; Division by zero
ISR_NOERRCODE 1   ; Debug
ISR_NOERRCODE 2   ; Non-maskable interrupt
ISR_NOERRCODE 3   ; Breakpoint
ISR_NOERRCODE 4   ; Overflow
ISR_NOERRCODE 5   ; Bound range exceeded
ISR_NOERRCODE 6   ; Invalid opcode
ISR_NOERRCODE 7   ; Device not available
ISR_ERRCODE   8   ; Double fault
ISR_NOERRCODE 9   ; Coprocessor segment overrun
ISR_ERRCODE   10  ; Invalid TSS
ISR_ERRCODE   11  ; Segment not present
ISR_ERRCODE   12  ; Stack-segment fault
ISR_ERRCODE   13  ; General protection fault
ISR_ERRCODE   14  ; Page fault
ISR_NOERRCODE 15  ; Reserved
ISR_NOERRCODE 16  ; x87 floating-point exception
ISR_NOERRCODE 17  ; Alignment check
ISR_NOERRCODE 18  ; Machine check
ISR_NOERRCODE 19  ; SIMD floating-point exception