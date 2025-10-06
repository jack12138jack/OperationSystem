#include "pit.h"
#include "ports.h"
#include "interrupts.h"

#define PIT_CHANNEL0 0x40
#define PIT_COMMAND  0x43
#define PIT_FREQUENCY 1193182u

static volatile uint64_t pit_tick_count = 0;
static uint32_t pit_current_frequency = 0;

static void pit_callback(registers_t *regs)
{
    UNUSED(regs);
    ++pit_tick_count;
}

void pit_init(uint32_t frequency)
{
    if (frequency == 0) {
        frequency = 100;
    }

    uint32_t divisor = PIT_FREQUENCY / frequency;
    if (divisor == 0) {
        divisor = 1;
    }

    outb(PIT_COMMAND, 0x36);
    outb(PIT_CHANNEL0, (uint8_t)(divisor & 0xFF));
    outb(PIT_CHANNEL0, (uint8_t)((divisor >> 8) & 0xFF));

    pit_tick_count = 0;
    pit_current_frequency = PIT_FREQUENCY / divisor;
    interrupts_register(32, pit_callback);
}

uint64_t pit_ticks(void)
{
    return pit_tick_count;
}

void pit_sleep(uint64_t ticks)
{
    uint64_t target = pit_ticks() + ticks;
    while (pit_ticks() < target) {
        __asm__ volatile ("hlt");
    }
}

uint32_t pit_frequency_hz(void)
{
    return pit_current_frequency;
}
