#include "boot_info.h"
#include "console.h"
#include "interrupts.h"
#include "keyboard.h"
#include "memory.h"
#include "pit.h"
#include "shell.h"
#include "system.h"

static void print_boot_info(const boot_info_t *info)
{
    if (!info || info->magic != BOOT_INFO_MAGIC) {
        console_write_line("[boot] 无效的引导信息");
        return;
    }

    console_write("[boot] 启动磁盘: 0x");
    console_write_hex(info->boot_drive);
    console_write("  载入扇区: ");
    console_write_dec(info->kernel_sectors);
    console_put_char('\n');
}

void kmain(boot_info_t *boot_info)
{
    console_init();
    system_store_boot_info(boot_info);
    console_write_line("==============================");
    console_write_line("  EDU OS 内核启动");
    console_write_line("==============================");

    print_boot_info(boot_info);

    interrupts_init();
    pit_init(100);
    keyboard_init();
    memory_init();
    interrupts_enable();

    console_write_line("核心服务已启动 (定时器/键盘/内存)");

    shell_run();

    for (;;) {
        __asm__ volatile ("hlt");
    }
}
