; bootloader.asm - Minimal bootloader for TKOS

BITS 16
ORG 0x7C00

KERNEL_OFFSET equ 0x1000
KERNEL_SECTORS equ 32      ; Number of sectors to load

start:
    ; Save boot drive number
    mov [boot_drive], dl

    ; Set up the stack
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00
    sti

    ; Print boot message
    mov si, msg_booting
    call print_string

    ; Reset disk system
    xor ax, ax
    mov dl, [boot_drive]
    int 0x13
    jc disk_error

load_kernel:
    mov si, msg_loading_kernel
    call print_string

    ; Set up es:bx to point to KERNEL_OFFSET
    mov ax, 0
    mov es, ax          ; Reset ES to 0
    mov bx, KERNEL_OFFSET ; ES:BX = 0:KERNEL_OFFSET

    ; Read from disk
    mov ah, 0x02        ; BIOS read sectors function
    mov al, KERNEL_SECTORS ; Number of sectors
    mov ch, 0           ; Cylinder 0
    mov dh, 0           ; Head 0
    mov cl, 2           ; Start from sector 2
    mov dl, [boot_drive]
    int 0x13
    jc disk_error

    mov si, msg_kernel_loaded
    call print_string

    ; Disable interrupts before switching to protected mode
    cli

    ; Load GDT
    mov si, msg_loading_gdt
    call print_string
    lgdt [gdt_descriptor]
    
    ; Enable A20 line
    in al, 0x92
    or al, 2
    out 0x92, al

    mov si, msg_switching_pm
    call print_string

    ; Switch to protected mode
    mov eax, cr0
    or al, 1
    mov cr0, eax
    
    ; Flush CPU pipeline with far jump
    jmp CODE_SEG:init_pm

disk_error:
    mov si, msg_disk_error
    call print_string
    jmp hang

[BITS 32]
init_pm:
    ; Set up segment registers
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    
    ; Set up stack
    mov esp, 0x90000

    ; Clear direction flag
    cld

    ; Jump to kernel
    jmp CODE_SEG:KERNEL_OFFSET

; Real mode functions
[BITS 16]
print_string:
    pusha
    mov ah, 0x0E
.next_char:
    lodsb
    test al, al        ; Check for null terminator
    jz .done
    int 0x10
    jmp .next_char
.done:
    popa
    ret

hang:
    cli                ; Disable interrupts
    hlt               ; Halt the CPU
    jmp hang          ; In case of NMI

; Data
align 4
msg_booting db 'Booting TKOS...', 13, 10, 0
msg_loading_kernel db 'Loading kernel...', 13, 10, 0
msg_kernel_loaded db 'Kernel loaded successfully!', 13, 10, 0
msg_loading_gdt db 'Loading GDT...', 13, 10, 0
msg_switching_pm db 'Switching to protected mode...', 13, 10, 0
msg_disk_error db 'Disk error! System halted.', 13, 10, 0
boot_drive db 0

; GDT
align 8
gdt_start:
    ; Null descriptor
    dq 0
    
    ; Code segment descriptor
    dw 0xFFFF    ; Limit (0-15)
    dw 0x0000    ; Base (0-15)
    db 0x00      ; Base (16-23)
    db 10011010b ; Access byte - Code
    db 11001111b ; Flags + Limit (16-19)
    db 0x00      ; Base (24-31)
    
    ; Data segment descriptor
    dw 0xFFFF    ; Limit (0-15)
    dw 0x0000    ; Base (0-15)
    db 0x00      ; Base (16-23)
    db 10010010b ; Access byte - Data
    db 11001111b ; Flags + Limit (16-19)
    db 0x00      ; Base (24-31)
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1  ; GDT size
    dd gdt_start                ; GDT address

CODE_SEG equ 8    ; Offset in GDT
DATA_SEG equ 16   ; Offset in GDT

; Padding and magic number
times 510-($-$$) db 0
dw 0xAA55
