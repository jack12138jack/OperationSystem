#include "console.h"
#include "ports.h"

static uint16_t *const VGA_BUFFER = (uint16_t *)0xB8000;
static uint8_t cursor_row = 0;
static uint8_t cursor_col = 0;
static uint8_t current_color = 0x07; /* 灰色前景，黑色背景 */

static uint16_t vga_entry(char c, uint8_t color)
{
    return (uint16_t)color << 8 | (uint8_t)c;
}

static void update_cursor(void)
{
    uint16_t pos = (uint16_t)cursor_row * VGA_WIDTH + cursor_col;
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

void console_set_color(uint8_t fg, uint8_t bg)
{
    current_color = (bg << 4) | (fg & 0x0F);
}

void console_clear(void)
{
    for (size_t y = 0; y < VGA_HEIGHT; ++y) {
        for (size_t x = 0; x < VGA_WIDTH; ++x) {
            VGA_BUFFER[y * VGA_WIDTH + x] = vga_entry(' ', current_color);
        }
    }
    cursor_row = cursor_col = 0;
    update_cursor();
}

void console_init(void)
{
    current_color = 0x07;
    console_clear();
}

void console_scroll(void)
{
    if (cursor_row < VGA_HEIGHT) {
        return;
    }

    for (size_t y = 1; y < VGA_HEIGHT; ++y) {
        for (size_t x = 0; x < VGA_WIDTH; ++x) {
            VGA_BUFFER[(y - 1) * VGA_WIDTH + x] = VGA_BUFFER[y * VGA_WIDTH + x];
        }
    }

    for (size_t x = 0; x < VGA_WIDTH; ++x) {
        VGA_BUFFER[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = vga_entry(' ', current_color);
    }

    cursor_row = VGA_HEIGHT - 1;
    if (cursor_col >= VGA_WIDTH) {
        cursor_col = VGA_WIDTH - 1;
    }
    update_cursor();
}

void console_put_char(char c)
{
    if (c == '\n') {
        cursor_col = 0;
        ++cursor_row;
    } else if (c == '\r') {
        cursor_col = 0;
    } else if (c == '\t') {
        cursor_col = (uint8_t)((cursor_col + 4) & ~(uint8_t)3);
    } else if (c == '\b') {
        console_backspace();
        return;
    } else {
        VGA_BUFFER[cursor_row * VGA_WIDTH + cursor_col] = vga_entry(c, current_color);
        ++cursor_col;
        if (cursor_col >= VGA_WIDTH) {
            cursor_col = 0;
            ++cursor_row;
        }
    }

    console_scroll();
    update_cursor();
}

void console_backspace(void)
{
    if (cursor_col == 0) {
        if (cursor_row == 0) {
            return;
        }
        cursor_col = VGA_WIDTH - 1;
        --cursor_row;
    } else {
        --cursor_col;
    }
    VGA_BUFFER[cursor_row * VGA_WIDTH + cursor_col] = vga_entry(' ', current_color);
    update_cursor();
}

void console_write(const char *str)
{
    while (*str) {
        console_put_char(*str++);
    }
}

void console_write_line(const char *str)
{
    console_write(str);
    console_put_char('\n');
}

void console_write_hex(uint32_t value)
{
    const char *hex = "0123456789ABCDEF";
    console_write("0x");
    for (int i = 7; i >= 0; --i) {
        uint8_t nibble = (value >> (i * 4)) & 0xF;
        console_put_char(hex[nibble]);
    }
}

void console_write_dec(uint32_t value)
{
    char buffer[12];
    int i = 0;
    if (value == 0) {
        console_put_char('0');
        return;
    }
    while (value > 0 && i < 11) {
        buffer[i++] = '0' + (value % 10);
        value /= 10;
    }
    while (i--) {
        console_put_char(buffer[i]);
    }
}
