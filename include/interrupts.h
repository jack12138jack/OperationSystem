#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include "common.h"

typedef struct registers {
    uint32_t gs, fs, es, ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags, useresp, ss;
} registers_t;

typedef void (*interrupt_handler_t)(registers_t *regs);

void interrupts_init(void);
void interrupts_register(uint8_t n, interrupt_handler_t handler);
void interrupts_enable(void);
void interrupts_disable(void);

#endif /* INTERRUPTS_H */
