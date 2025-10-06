#include "interrupts.h"
#include "console.h"
#include "idt.h"
#include "pic.h"

static interrupt_handler_t handlers[256];

static const char *exception_messages[32] = {
    "Divide By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Overflow",
    "Bound Range Exceeded",
    "Invalid Opcode",
    "Device Not Available",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Invalid TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Reserved",
    "x87 Floating Point",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating Point",
    "Virtualization",
    "Control Protection",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

void interrupts_register(uint8_t n, interrupt_handler_t handler)
{
    handlers[n] = handler;
}

void interrupts_enable(void)
{
    __asm__ volatile ("sti");
}

void interrupts_disable(void)
{
    __asm__ volatile ("cli");
}

void isr_handler(registers_t *regs)
{
    if (regs->int_no < 32) {
        console_write_line("==== CPU 异常 ====");
        console_write("向量: ");
        console_write_dec(regs->int_no);
        console_write("  错误码: ");
        console_write_hex(regs->err_code);
        console_write("\n原因: ");
        console_write_line(exception_messages[regs->int_no]);
        console_write_line("系统挂起");
        interrupts_disable();
        for (;;) {
            __asm__ volatile ("hlt");
        }
    }

    if (handlers[regs->int_no]) {
        handlers[regs->int_no](regs);
    }
}

void irq_handler(registers_t *regs)
{
    uint8_t irq = (uint8_t)(regs->int_no - 32);

    if (handlers[regs->int_no]) {
        handlers[regs->int_no](regs);
    }

    pic_send_eoi(irq);
}

void interrupts_init(void)
{
    for (size_t i = 0; i < 256; ++i) {
        handlers[i] = 0;
    }

    pic_init();
    idt_init();
}
