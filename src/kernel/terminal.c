#include "terminal.h"
#include "port.h"

static uint16_t* const VGA_MEMORY = (uint16_t*)0xB8000;
static uint8_t terminal_color = COLOR_LIGHT_GREY | (COLOR_BLACK << 4);
static size_t terminal_row = 0;
static size_t terminal_column = 0;

static uint8_t make_color(enum vga_color fg, enum vga_color bg) {
    return fg | bg << 4;
}

static uint16_t make_vgaentry(char c, uint8_t color) {
    uint16_t c16 = (uint16_t)c;
    uint16_t color16 = (uint16_t)color;
    return c16 | (color16 << 8);
}

static void terminal_update_cursor(void) {
    uint16_t position = (uint16_t)(terminal_row * VGA_WIDTH + terminal_column);
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(position & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((position >> 8) & 0xFF));
}

static void terminal_scroll(void) {
    if (terminal_row < VGA_HEIGHT) {
        return;
    }

    size_t row, col;
    for (row = 1; row < VGA_HEIGHT; row++) {
        for (col = 0; col < VGA_WIDTH; col++) {
            VGA_MEMORY[(row - 1) * VGA_WIDTH + col] = VGA_MEMORY[row * VGA_WIDTH + col];
        }
    }

    for (col = 0; col < VGA_WIDTH; col++) {
        VGA_MEMORY[(VGA_HEIGHT - 1) * VGA_WIDTH + col] = make_vgaentry(' ', terminal_color);
    }

    terminal_row = VGA_HEIGHT - 1;
    terminal_column = 0;
}

void terminal_initialize(void) {
    terminal_color = make_color(COLOR_LIGHT_GREY, COLOR_BLACK);
    terminal_row = 0;
    terminal_column = 0;

    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            VGA_MEMORY[y * VGA_WIDTH + x] = make_vgaentry(' ', terminal_color);
        }
    }
    terminal_update_cursor();
}

void terminal_setcolor(uint8_t color) {
    terminal_color = color;
}

void terminal_putchar(char c) {
    if (c == '\n') {
        terminal_column = 0;
        terminal_row++;
    } else if (c == '\b') {
        if (terminal_column > 0) {
            terminal_column--;
        } else if (terminal_row > 0) {
            terminal_row--;
            terminal_column = VGA_WIDTH - 1;
        }
        VGA_MEMORY[terminal_row * VGA_WIDTH + terminal_column] = make_vgaentry(' ', terminal_color);
    } else {
        VGA_MEMORY[terminal_row * VGA_WIDTH + terminal_column] = make_vgaentry(c, terminal_color);
        terminal_column++;
        if (terminal_column >= VGA_WIDTH) {
            terminal_column = 0;
            terminal_row++;
        }
    }

    terminal_scroll();
    terminal_update_cursor();
}

void terminal_write(const char* data) {
    for (size_t i = 0; data[i] != '\0'; i++) {
        terminal_putchar(data[i]);
    }
}

void terminal_writeline(const char* data) {
    terminal_write(data);
    terminal_putchar('\n');
}

void terminal_clear(void) {
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            VGA_MEMORY[y * VGA_WIDTH + x] = make_vgaentry(' ', terminal_color);
        }
    }
    terminal_row = 0;
    terminal_column = 0;
    terminal_update_cursor();
}

void terminal_backspace(void) {
    terminal_putchar('\b');
}
