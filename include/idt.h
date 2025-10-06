#ifndef IDT_H
#define IDT_H

#include "common.h"

typedef struct __attribute__((packed)) idt_entry {
    uint16_t base_low;
    uint16_t sel;
    uint8_t always0;
    uint8_t flags;
    uint16_t base_high;
} idt_entry_t;

typedef struct __attribute__((packed)) idt_ptr {
    uint16_t limit;
    uint32_t base;
} idt_ptr_t;

void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags);
void idt_init(void);

#endif /* IDT_H */
