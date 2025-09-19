#include "command.h"
#include "terminal.h"
#include "keyboard.h"
#include "util.h"
#include "port.h"

static const char* HELP_TEXT[] = {
    "Available commands:",
    "  help           - Show this help message",
    "  about          - Learn about EduOS",
    "  clear          - Clear the screen",
    "  lessons        - List all teaching modules",
    "  lesson <n>     - Open a specific lesson",
    "  memory         - Display memory layout notes",
    "  reboot         - Attempt to reboot the machine",
    0
};

static const char* LESSONS[] = {
    "1. Boot process: BIOS reads the first sector and executes it.",
    "2. Protected mode: switch from real-mode to access more memory.",
    "3. Memory map: VGA text memory sits at 0xB8000.",
    "4. Interrupts: the CPU reacts to events via the IDT.",
    "5. Scheduling: an OS selects which task to run next.",
    "6. System calls: user programs request kernel services.",
    "7. File systems: data is organized into files and directories.",
    "8. Drivers: hardware is controlled through small kernel modules.",
    "9. Virtual memory: map addresses to isolate and protect tasks.",
    "10. User space: applications run with limited privileges.",
};

static void print_help(void) {
    for (size_t i = 0; HELP_TEXT[i] != 0; i++) {
        terminal_writeline(HELP_TEXT[i]);
    }
}

static void print_about(void) {
    terminal_writeline("EduOS - a teaching operating system kernel");
    terminal_writeline("Built to demonstrate the basics of low-level programming.");
    terminal_writeline("Source layout:");
    terminal_writeline("  boot.asm   -> real-mode bootloader");
    terminal_writeline("  kernel/*   -> protected-mode C kernel");
    terminal_writeline("Try 'lessons' for structured study topics.");
}

static void list_lessons(void) {
    terminal_writeline("Available lessons:");
    for (size_t i = 0; i < (sizeof(LESSONS) / sizeof(LESSONS[0])); i++) {
        terminal_writeline(LESSONS[i]);
    }
    terminal_writeline("Use 'lesson <n>' to expand a specific topic.");
}

static void show_lesson(size_t number) {
    if (number == 0 || number > (sizeof(LESSONS) / sizeof(LESSONS[0]))) {
        terminal_writeline("Lesson not found. Try 'lessons' to list options.");
        return;
    }

    terminal_writeline("Lesson details:");
    terminal_writeline(LESSONS[number - 1]);

    switch (number) {
        case 1:
            terminal_writeline("- Understand BIOS responsibilities and boot sectors.");
            terminal_writeline("- Explore how the bootloader loads the kernel.");
            break;
        case 2:
            terminal_writeline("- Observe GDT setup and CR0 flag toggling.");
            terminal_writeline("- Why do we need 32-bit mode for C code?");
            break;
        case 3:
            terminal_writeline("- VGA memory is 80x25 characters with attributes.");
            terminal_writeline("- Experiment by editing terminal.c to change colors.");
            break;
        case 4:
            terminal_writeline("- Extend the kernel by adding an IDT and timer interrupt.");
            terminal_writeline("- Consider how interrupts preempt running code.");
            break;
        case 5:
            terminal_writeline("- Implement a simple round-robin scheduler skeleton.");
            terminal_writeline("- Think about context switching registers.");
            break;
        case 6:
            terminal_writeline("- Define an interrupt gate to handle software traps.");
            terminal_writeline("- Discuss user versus kernel privilege levels.");
            break;
        case 7:
            terminal_writeline("- Start with a RAM-backed file table for experimentation.");
            terminal_writeline("- Filesystems balance performance and reliability.");
            break;
        case 8:
            terminal_writeline("- Device registers are memory-mapped or port-mapped.");
            terminal_writeline("- Explore keyboard.c for a minimal driver example.");
            break;
        case 9:
            terminal_writeline("- Paging translates virtual addresses into physical ones.");
            terminal_writeline("- Introduce page tables to isolate applications.");
            break;
        case 10:
            terminal_writeline("- User space enables isolation and stability.");
            terminal_writeline("- What mechanisms enforce privilege separation?");
            break;
        default:
            break;
    }
}

static void show_memory_notes(void) {
    terminal_writeline("Memory map hints:");
    terminal_writeline("  0x00007C00 -> bootloader execution address");
    terminal_writeline("  0x00010000 -> kernel load address");
    terminal_writeline("  0x00090000 -> stack top during early boot");
    terminal_writeline("  0x000B8000 -> VGA text buffer");
    terminal_writeline("Try modifying linker.ld to explore different layouts.");
}

static void attempt_reboot(void) {
    terminal_writeline("Rebooting via keyboard controller...");
    outb(0x64, 0xFE);
    terminal_writeline("If reboot fails, reset the emulator manually.");
}

void command_execute(const char* input) {
    if (input == NULL || input[0] == '\0') {
        return;
    }

    if (string_compare(input, "help") == 0) {
        print_help();
    } else if (string_compare(input, "about") == 0) {
        print_about();
    } else if (string_compare(input, "clear") == 0) {
        terminal_clear();
    } else if (string_compare(input, "lessons") == 0) {
        list_lessons();
    } else if (string_starts_with(input, "lesson")) {
        if (input[6] == '\0') {
            terminal_writeline("Usage: lesson <number>");
            return;
        }
        size_t i = 6;
        while (input[i] == ' ') {
            i++;
        }
        int number = 0;
        if (!string_to_int(&input[i], &number) || number <= 0) {
            terminal_writeline("Please provide a valid lesson number.");
            return;
        }
        show_lesson((size_t)number);
    } else if (string_compare(input, "memory") == 0) {
        show_memory_notes();
    } else if (string_compare(input, "reboot") == 0) {
        attempt_reboot();
    } else {
        terminal_writeline("Unknown command. Type 'help' to see available options.");
    }
}

void shell_run(void) {
    char buffer[128];
    while (1) {
        terminal_setcolor(COLOR_LIGHT_GREEN | (COLOR_BLACK << 4));
        terminal_write("EduOS> ");
        terminal_setcolor(COLOR_LIGHT_GREY | (COLOR_BLACK << 4));
        keyboard_read_line(buffer, sizeof(buffer));
        command_execute(buffer);
    }
}
