#ifndef EDUOS_TERMINAL_H
#define EDUOS_TERMINAL_H

#include "common.h"

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

enum vga_color {
    COLOR_BLACK = 0,
    COLOR_BLUE = 1,
    COLOR_GREEN = 2,
    COLOR_CYAN = 3,
    COLOR_RED = 4,
    COLOR_MAGENTA = 5,
    COLOR_BROWN = 6,
    COLOR_LIGHT_GREY = 7,
    COLOR_DARK_GREY = 8,
    COLOR_LIGHT_BLUE = 9,
    COLOR_LIGHT_GREEN = 10,
    COLOR_LIGHT_CYAN = 11,
    COLOR_LIGHT_RED = 12,
    COLOR_LIGHT_MAGENTA = 13,
    COLOR_LIGHT_BROWN = 14,
    COLOR_WHITE = 15,
};

void terminal_initialize(void);
void terminal_setcolor(uint8_t color);
void terminal_write(const char* data);
void terminal_writeline(const char* data);
void terminal_putchar(char c);
void terminal_clear(void);
void terminal_backspace(void);

#endif /* EDUOS_TERMINAL_H */
