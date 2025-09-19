#include "terminal.h"
#include "keyboard.h"
#include "command.h"

void kernel_main(void) {
    terminal_initialize();
    keyboard_initialize();

    terminal_setcolor(COLOR_LIGHT_CYAN | (COLOR_BLACK << 4));
    terminal_writeline("Welcome to EduOS");
    terminal_setcolor(COLOR_LIGHT_GREY | (COLOR_BLACK << 4));
    terminal_writeline("An educational operating system for learning systems programming.");
    terminal_writeline("Type 'help' to explore available lessons and tools.");

    shell_run();
}
