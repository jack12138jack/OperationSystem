; boot.asm - 16-bit bootloader for EduOS
; Loads the protected mode kernel and jumps to it.

[org 0x7c00]
[bits 16]

CODE_SEG equ 0x08
DATA_SEG equ 0x10

start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00

    mov [boot_drive], dl

    call enable_a20
    call load_kernel
    call enter_protected_mode

hang:
    hlt
    jmp hang

; ----------------------
; BIOS printing helpers
; ----------------------
print_string:
    pusha
.next_char:
    lodsb
    cmp al, 0
    je .done
    mov ah, 0x0e
    mov bh, 0
    mov bl, 0x07
    int 0x10
    jmp .next_char
.done:
    popa
    ret

print_hex:
    pusha
    mov cx, 4
.hex_loop:
    rol ax, 4
    mov bx, ax
    and bx, 0x000f
    cmp bl, 0x0a
    jl .digit
    add bl, 'A' - 0x0a
    jmp .emit
.digit:
    add bl, '0'
.emit:
    mov ah, 0x0e
    mov al, bl
    mov bh, 0
    mov bl, 0x07
    int 0x10
    loop .hex_loop
    popa
    ret

; ----------------------
; Load kernel using BIOS
; ----------------------
load_kernel:
    pusha
    mov ax, kernel_load_address
    mov es, ax
    xor bx, bx
    mov ah, 0x02            ; BIOS read sectors
    mov al, KERNEL_SECTORS
    mov ch, 0x00
    mov cl, 0x02            ; start from second sector
    mov dh, 0x00
    mov dl, [boot_drive]
    int 0x13
    jc disk_error
    popa
    ret

disk_error:
    mov si, disk_error_msg
    call print_string
    jmp hang

; ----------------------
; Enable A20 line
; ----------------------
enable_a20:
    in al, 0x92
    test al, 0x02
    jnz .done
    or al, 0x02
    out 0x92, al
.done:
    ret

; ----------------------
; Switch to protected mode
; ----------------------
enter_protected_mode:
    cli
    lgdt [gdt_descriptor]
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax
    jmp CODE_SEG:init_pm

[bits 32]
init_pm:
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x0090000      ; stack in low memory
    jmp CODE_SEG:kernel_entry_point

kernel_entry_point equ 0x0010000
kernel_load_address equ 0x1000

; ----------------------
; Global Descriptor Table
; ----------------------
align 8
gdt_start:
    dq 0x0000000000000000
    dq 0x00cf9a000000ffff
    dq 0x00cf92000000ffff
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

[bits 16]
boot_drive db 0
KERNEL_SECTORS equ 64

disk_error_msg db "Disk read error!", 0

; Boot signature
    times 510-($-$$) db 0
    dw 0xaa55
