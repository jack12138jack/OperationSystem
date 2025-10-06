.section .text
.intel_syntax noprefix
.globl idt_flush

idt_flush:
    mov eax, [esp + 4]
    lidt [eax]
    ret

.section .note.GNU-stack,"",@progbits
