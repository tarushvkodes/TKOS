; bootloader.asm - Minimal bootloader for TKOS

BITS 16
ORG 0x7C00

KERNEL_OFFSET equ 0x1000
KERNEL_SECTORS equ 32      ; Number of sectors to load

; GDT
gdt_start:
    ; Null descriptor
    dd 0x0
    dd 0x0
    
    ; Code segment
    dw 0xffff    ; Limit
    dw 0x0       ; Base
    db 0x0       ; Base
    db 10011010b ; Access
    db 11001111b ; Flags + Limit
    db 0x0       ; Base
    
    ; Data segment
    dw 0xffff    ; Limit
    dw 0x0       ; Base
    db 0x0       ; Base
    db 10010010b ; Access
    db 11001111b ; Flags + Limit
    db 0x0       ; Base
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1 ; Size
    dd gdt_start               ; Start address

CODE_SEG equ 0x08
DATA_SEG equ 0x10

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

    ; Switch to protected mode
    cli
    lgdt [gdt_descriptor]
    
    ; Enable A20 line
    in al, 0x92
    or al, 2
    out 0x92, al

    ; Switch to protected mode
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    
    ; Far jump to flush pipeline and load CS
    jmp CODE_SEG:protected_mode

BITS 32
protected_mode:
    ; Set up segment registers
    mov ax, DATA_SEG
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Set up stack
    mov ebp, 0x90000
    mov esp, ebp

    ; Jump to kernel
    jmp KERNEL_OFFSET

; Error handlers and utility functions
disk_error:
    mov si, msg_disk_error
    call print_string
    jmp $

print_string:
    pusha
    mov ah, 0x0e    ; BIOS teletype function
.repeat:
    lodsb           ; Load byte from SI into AL
    test al, al     ; Check if character is null
    jz .done        ; If yes, we're done
    int 0x10        ; Print character
    jmp .repeat     ; Repeat for next character
.done:
    popa
    ret

; Data
boot_drive: db 0
msg_booting: db "Booting TKOS...", 13, 10, 0
msg_loading_kernel: db "Loading kernel...", 13, 10, 0
msg_kernel_loaded: db "Kernel loaded.", 13, 10, 0
msg_disk_error: db "Disk error!", 13, 10, 0

; Padding and boot signature
times 510 - ($ - $$) db 0
dw 0xaa55
