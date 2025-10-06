#include "keyboard.h"
#include "port.h"
#include "terminal.h"

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64

#define SCANCODE_BACKSPACE 0x0E
#define SCANCODE_ENTER 0x1C
#define SCANCODE_LEFT_SHIFT 0x2A
#define SCANCODE_RIGHT_SHIFT 0x36
#define SCANCODE_CAPS_LOCK 0x3A

static bool shift_active = false;
static bool caps_lock_active = false;

static const char keymap[128] = {
    0,  27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n', 0,
    'a','s','d','f','g','h','j','k','l',';','\'', '`', 0, '\\',
    'z','x','c','v','b','n','m',',','.','/', 0, '*', 0, ' ',
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static const char keymap_shift[128] = {
    0,  27, '!','@','#','$','%','^','&','*','(',')','_','+', '\b',
    '\t','Q','W','E','R','T','Y','U','I','O','P','{','}','\n', 0,
    'A','S','D','F','G','H','J','K','L',':','"','~', 0, '|',
    'Z','X','C','V','B','N','M','<','>','?', 0, '*', 0, ' ',
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

void keyboard_initialize(void) {
    shift_active = false;
    caps_lock_active = false;
}

static bool keyboard_has_data(void) {
    return (inb(KEYBOARD_STATUS_PORT) & 0x01) != 0;
}

static char translate_scancode(uint8_t scancode) {
    if (scancode & 0x80) {
        return 0;
    }

    char c;
    bool upper = shift_active ^ caps_lock_active;

    if (upper) {
        c = keymap_shift[scancode];
        if (c == 0) {
            c = keymap[scancode];
            if (c >= 'a' && c <= 'z') {
                c = (char)(c - 'a' + 'A');
            }
        }
    } else {
        c = keymap[scancode];
    }

    return c;
}

char keyboard_get_char(void) {
    while (1) {
        if (!keyboard_has_data()) {
            continue;
        }

        uint8_t scancode = inb(KEYBOARD_DATA_PORT);

        if (scancode == SCANCODE_LEFT_SHIFT || scancode == SCANCODE_RIGHT_SHIFT) {
            shift_active = true;
            continue;
        }
        if (scancode == (SCANCODE_LEFT_SHIFT | 0x80) || scancode == (SCANCODE_RIGHT_SHIFT | 0x80)) {
            shift_active = false;
            continue;
        }
        if (scancode == SCANCODE_CAPS_LOCK) {
            caps_lock_active = !caps_lock_active;
            continue;
        }

        if (scancode & 0x80) {
            continue;
        }

        if (scancode == SCANCODE_BACKSPACE) {
            return '\b';
        }
        if (scancode == SCANCODE_ENTER) {
            return '\n';
        }

        char c = translate_scancode(scancode);
        if (c != 0) {
            return c;
        }
    }
}

void keyboard_read_line(char* buffer, size_t max_length) {
    size_t length = 0;

    while (1) {
        char c = keyboard_get_char();
        if (c == '\n') {
            buffer[length] = '\0';
            terminal_putchar('\n');
            return;
        } else if (c == '\b') {
            if (length > 0) {
                length--;
                terminal_backspace();
            }
        } else {
            if (length + 1 < max_length) {
                buffer[length++] = c;
                terminal_putchar(c);
            }
        }
    }
}
