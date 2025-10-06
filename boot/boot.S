/*
 * 简易教育操作系统的引导扇区
 */

.section .text
.code16
.intel_syntax noprefix
.globl _start

.set CODE_SEG,        0x08
.set DATA_SEG,        0x10
.set STACK_TOP,       0x7C00
.set KERNEL_LOAD_SEG, 0x1000
.set KERNEL_LOAD_OFF, 0x0000
.set KERNEL_ENTRY,    0x0010000

_start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, STACK_TOP
    sti

    mov [boot_drive], dl

    call load_kernel
    call enable_a20
    call build_boot_info
    call enter_protected_mode

hang:
    hlt
    jmp hang

load_kernel:
    lea si, [disk_address_packet]
    mov ax, [kernel_sectors]
    cmp ax, 0
    jne lk_have_sectors
    mov ax, 1
    mov [kernel_sectors], ax
lk_have_sectors:
    mov [si + 2], ax          /* 更新扇区数 */
    mov dl, [boot_drive]
    mov ah, 0x42
    int 0x13
    jc disk_error
    ret

disk_error:
    mov si, offset disk_error_msg
.disk_loop:
    lodsb
    cmp al, 0
    je hang
    mov ah, 0x0E
    mov bx, 0x0007
    int 0x10
    jmp .disk_loop

enable_a20:
    in al, 0x92
    test al, 0x02
    jnz .done
    or al, 0x02
    out 0x92, al
.wait:
    in al, 0x92
    test al, 0x02
    jz .wait
.done:
    ret

build_boot_info:
    mov al, [boot_drive]
    mov [boot_info_drive], al
    mov ax, [kernel_sectors]
    mov [boot_info_kernel_sectors], ax
    mov word ptr [boot_info_kernel_segment], KERNEL_LOAD_SEG
    mov word ptr [boot_info_kernel_offset], KERNEL_LOAD_OFF
    mov dword ptr [boot_info_kernel_entry], KERNEL_ENTRY
    ret

enter_protected_mode:
    cli
    lgdt [gdt_descriptor]
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax
    jmp CODE_SEG:protected_entry

.code32
protected_entry:
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov fs, ax
    mov gs, ax

    mov esp, 0x0090000

    push boot_info             /* 传递引导信息给内核 */
    mov edx, KERNEL_ENTRY
    call edx

halt32:
    cli
    hlt
    jmp halt32

/* -------------------- 数据与结构 -------------------- */

.code16
boot_drive:
    .byte 0
    .byte 0

.align 4
boot_info:
    .long 0x45444F53           /* "EDOS" 标识 */
boot_info_drive:
    .byte 0
    .byte 0
boot_info_kernel_sectors:
    .word 0
boot_info_kernel_segment:
    .word KERNEL_LOAD_SEG
boot_info_kernel_offset:
    .word KERNEL_LOAD_OFF
boot_info_kernel_entry:
    .long KERNEL_ENTRY

.align 4
disk_address_packet:
    .byte 0x10
    .byte 0x00
kernel_sectors:
    .word 0
    .word KERNEL_LOAD_OFF
    .word KERNEL_LOAD_SEG
    .long 1
    .long 0

disk_error_msg:
    .ascii "Disk read error!\r\n"
    .byte 0

.align 8
gdt:
    .quad 0x0000000000000000
    .quad 0x00CF9A000000FFFF
    .quad 0x00CF92000000FFFF

gdt_end:

gdt_descriptor:
    .word gdt_end - gdt - 1
    .long gdt

.org 0x1FE
    .word 0xAA55
