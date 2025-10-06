#ifndef PORTS_H
#define PORTS_H

#include "common.h"

static inline void outb(uint16_t port, uint8_t value)
{
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

static inline uint8_t inb(uint16_t port)
{
    uint8_t value;
    __asm__ volatile ("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

static inline void io_wait(void)
{
    /* 利用端口 0x80 的写入作为等待 */
    __asm__ volatile ("outb %%al, $0x80" : : "a"(0));
}

#endif /* PORTS_H */
