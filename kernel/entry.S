/* 内核入口，负责清理环境并调用 C 代码 */

.section .text
.intel_syntax noprefix
.globl _start
.extern kmain
.extern __bss_start
.extern __bss_end

_start:
    cld

    /* 取出引导信息指针（在返回地址之后） */
    mov ebx, [esp + 4]

    /* 丢弃从引导器来的返回地址，重建栈 */
    add esp, 4

    /* 统一段寄存器 */
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    /* 清零 .bss 段 */
    mov edi, __bss_start
    mov ecx, __bss_end
    sub ecx, edi
    mov eax, 0
    rep stosb

    /* 重新设置栈顶 */
    mov esp, 0x0090000
    mov ebp, 0

    /* 将引导信息作为第一个参数压栈，调用 C 内核 */
    push ebx
    call kmain

halt_loop:
    cli
    hlt
    jmp halt_loop

.section .note.GNU-stack,"",@progbits
