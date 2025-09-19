[bits 32]

extern kernel_main

global kernel_entry

kernel_entry:
    mov esp, 0x0090000
    call kernel_main

.hang:
    hlt
    jmp .hang

section .note.GNU-stack noalloc noexec nowrite align=1
