#include "keyboard.h"
#include "interrupts.h"
#include "ports.h"

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_BUFFER_SIZE 128

static volatile char buffer[KEYBOARD_BUFFER_SIZE];
static volatile uint8_t head = 0;
static volatile uint8_t tail = 0;
static bool shift_pressed = false;
static bool caps_lock = false;

static const char scancode_table[] = {
    0,   27, '1', '2', '3', '4', '5', '6',
    '7', '8', '9', '0', '-', '=', '\b', '\t',
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',
    'o', 'p', '[', ']', '\n', 0,   'a', 's',
    'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
    '\'', '`', 0,  '\\', 'z', 'x', 'c', 'v',
    'b', 'n', 'm', ',', '.', '/', 0,   '*',
    0,   ' ', 0,    0,    0,    0,    0,    0,
    0,   0,   0,    0,    0,    '-',  0,    0,
    0,   '+', 0,    0,    0,    0,    0,    0,
    0,   0,   0,    0,    0,    0,    0,    0,
    0,   0,   0,    0,    0,    0,    0,    0,
    0,   0,   0,    0,    0,    0,    0,    0,
    0,   0,   0,    0
};

static const char scancode_table_shift[] = {
    0,   27, '!', '@', '#', '$', '%', '^',
    '&', '*', '(', ')', '_', '+', '\b', '\t',
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I',
    'O', 'P', '{', '}', '\n', 0,   'A', 'S',
    'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',
    '"', '~', 0,  '|', 'Z', 'X', 'C', 'V',
    'B', 'N', 'M', '<', '>', '?', 0,   '*',
    0,   ' ', 0,    0,    0,    0,    0,    0,
    0,   0,   0,    0,    0,    '_',  0,    0,
    0,   '+', 0,    0,    0,    0,    0,    0,
    0,   0,   0,    0,    0,    0,    0,    0,
    0,   0,   0,    0,    0,    0,    0,    0,
    0,   0,   0,    0,    0,    0,    0,    0,
    0,   0,   0,    0
};

static void buffer_push(char c)
{
    uint8_t next = (uint8_t)((head + 1) % KEYBOARD_BUFFER_SIZE);
    if (next == tail) {
        return; /* 缓冲区满时丢弃 */
    }
    buffer[head] = c;
    head = next;
}

static bool buffer_pop(char *c)
{
    if (head == tail) {
        return false;
    }
    *c = buffer[tail];
    tail = (uint8_t)((tail + 1) % KEYBOARD_BUFFER_SIZE);
    return true;
}

static void handle_scancode(uint8_t scancode)
{
    if (scancode & 0x80) {
        /* 按键释放 */
        scancode &= 0x7F;
        if (scancode == 0x2A || scancode == 0x36) {
            shift_pressed = false;
        }
        return;
    }

    switch (scancode) {
        case 0x2A:
        case 0x36:
            shift_pressed = true;
            return;
        case 0x3A:
            caps_lock = !caps_lock;
            return;
        default:
            break;
    }

    char ch = 0;
    if (scancode < sizeof(scancode_table)) {
        bool use_shift = shift_pressed;
        if (scancode_table[scancode] >= 'a' && scancode_table[scancode] <= 'z') {
            use_shift ^= caps_lock;
        }
        ch = use_shift ? scancode_table_shift[scancode] : scancode_table[scancode];
    }

    if (ch) {
        buffer_push(ch);
    }
}

static void keyboard_irq(registers_t *regs)
{
    UNUSED(regs);
    uint8_t scancode = inb(KEYBOARD_DATA_PORT);
    handle_scancode(scancode);
}

void keyboard_init(void)
{
    head = tail = 0;
    shift_pressed = false;
    caps_lock = false;
    interrupts_register(33, keyboard_irq);
}

bool keyboard_has_char(void)
{
    return head != tail;
}

char keyboard_get_char(void)
{
    char c;
    while (!buffer_pop(&c)) {
        __asm__ volatile ("hlt");
    }
    return c;
}
