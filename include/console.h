#ifndef CONSOLE_H
#define CONSOLE_H

#include "common.h"

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

void console_init(void);
void console_clear(void);
void console_set_color(uint8_t fg, uint8_t bg);
void console_write(const char *str);
void console_write_line(const char *str);
void console_write_hex(uint32_t value);
void console_write_dec(uint32_t value);
void console_put_char(char c);
void console_backspace(void);
void console_scroll(void);

#endif /* CONSOLE_H */
