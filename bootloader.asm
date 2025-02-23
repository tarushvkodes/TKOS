; bootloader.asm - Minimal bootloader for TKOS
BITS 16
ORG 0x7C00

KERNEL_OFFSET equ 0x1000
KERNEL_SECTORS equ 64      ; Total sectors to read
MAX_SECTORS_PER_READ equ 16 ; Maximum sectors to read in one operation

; GDT
gdt_start:
    dd 0x0, 0x0           ; Null descriptor
    db 0xFF, 0xFF, 0x00, 0x00, 0x00, 10011010b, 11001111b, 0x00  ; Code segment
    db 0xFF, 0xFF, 0x00, 0x00, 0x00, 10010010b, 11001111b, 0x00  ; Data segment
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

CODE_SEG equ 0x08
DATA_SEG equ 0x10

start:
    mov [boot_drive], dl
    
    ; Set up stack
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00
    sti

    ; Reset disk system (3 attempts)
    mov cx, 3
.retry_reset:
    xor ax, ax
    mov dl, [boot_drive]
    int 0x13
    jnc load_kernel
    loop .retry_reset
    jmp disk_error

load_kernel:
    xor ax, ax
    mov es, ax
    mov bx, KERNEL_OFFSET
    mov word [sectors_read], 0
    mov byte [current_track], 0
    mov byte [current_head], 0
    mov byte [current_sector], 2

read_next_chunk:
    mov ax, KERNEL_SECTORS
    sub ax, [sectors_read]
    jz protected_mode_switch
    
    cmp ax, MAX_SECTORS_PER_READ
    jbe .do_read
    mov ax, MAX_SECTORS_PER_READ

.do_read:
    push ax
    mov ah, 0x02
    mov al, [esp]
    mov ch, [current_track]
    mov cl, [current_sector]
    mov dh, [current_head]
    mov dl, [boot_drive]
    
    mov si, 3
.retry_read:
    pusha
    int 0x13
    jnc .read_ok
    
    popa
    dec si
    jz disk_error
    
    xor ax, ax
    int 0x13
    jmp .retry_read

.read_ok:
    popa
    pop ax
    
    add [sectors_read], ax
    
    ; Update buffer pointer (bx += sectors * 512)
    mov cx, ax
    shl ax, 9
    add bx, ax
    
    ; Update CHS
    call update_chs
    jmp read_next_chunk

update_chs:
    inc byte [current_sector]
    cmp byte [current_sector], 19
    jle .done
    
    mov byte [current_sector], 1
    inc byte [current_head]
    cmp byte [current_head], 2
    jl .done
    
    mov byte [current_head], 0
    inc byte [current_track]
.done:
    ret

protected_mode_switch:
    cli
    lgdt [gdt_descriptor]
    
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    
    jmp CODE_SEG:protected_mode

disk_error:
    mov si, msg_error
    call print_string
    jmp $

print_string:
    mov ah, 0x0E
.next:
    lodsb
    test al, al
    jz .done
    int 0x10
    jmp .next
.done:
    ret

; Data
align 4
sectors_read: dw 0
current_track: db 0
current_head: db 0
current_sector: db 2
boot_drive: db 0
msg_error: db "Disk error", 13, 10, 0

; Protected mode
BITS 32
protected_mode:
    mov ax, DATA_SEG
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    mov ebp, 0x90000
    mov esp, ebp
    
    jmp KERNEL_OFFSET

; Boot signature
times 510 - ($ - $$) db 0
dw 0xAA55
