#include "shell.h"

#include "common.h"
#include "console.h"
#include "interrupts.h"
#include "keyboard.h"
#include "memory.h"
#include "pit.h"
#include "system.h"

#include <stdbool.h>

#define INPUT_BUFFER_SIZE 128

typedef void (*command_handler_t)(const char *args);

typedef struct shell_command {
    const char *name;
    const char *description;
    command_handler_t handler;
} shell_command_t;

static void cmd_help(const char *args);
static void cmd_info(const char *args);
static void cmd_clear(const char *args);
static void cmd_echo(const char *args);
static void cmd_ticks(const char *args);
static void cmd_sleep(const char *args);
static void cmd_mem(const char *args);
static void cmd_alloc(const char *args);
static void cmd_uptime(const char *args);
static void cmd_halt(const char *args);

static const shell_command_t commands[] = {
    {"help",  "列出所有命令",          cmd_help},
    {"info",  "显示系统信息",          cmd_info},
    {"clear", "清空屏幕",              cmd_clear},
    {"echo",  "回显文本",              cmd_echo},
    {"ticks", "查看定时器节拍计数",    cmd_ticks},
    {"sleep", "sleep <ms> - 休眠指定毫秒", cmd_sleep},
    {"mem",   "查看简易内存统计",      cmd_mem},
    {"alloc", "alloc <bytes> - 申请内存", cmd_alloc},
    {"uptime","显示运行时间 (秒)",       cmd_uptime},
    {"halt",  "停止系统",              cmd_halt}
};

static int str_compare(const char *a, const char *b)
{
    while (*a && *b && *a == *b) {
        ++a;
        ++b;
    }
    return (int)((unsigned char)*a - (unsigned char)*b);
}

static const char *skip_spaces(const char *s)
{
    while (*s == ' ' || *s == '\t') {
        ++s;
    }
    return s;
}

static bool parse_uint(const char *s, uint32_t *out)
{
    uint32_t value = 0;
    s = skip_spaces(s);
    if (!*s) {
        return false;
    }
    while (*s >= '0' && *s <= '9') {
        value = value * 10u + (uint32_t)(*s - '0');
        ++s;
    }
    if (*s && *s != ' ' && *s != '\t') {
        return false;
    }
    *out = value;
    return true;
}

static uint64_t div_u64_u32(uint64_t value, uint32_t divisor)
{
    if (divisor == 0) {
        return 0;
    }

    uint64_t quotient = 0;
    uint64_t remainder = 0;
    for (int i = 63; i >= 0; --i) {
        remainder = (remainder << 1) | ((value >> i) & 1u);
        if (remainder >= divisor) {
            remainder -= divisor;
            quotient |= (uint64_t)1 << i;
        }
    }
    return quotient;
}

static size_t read_line(char *buffer, size_t max_len)
{
    size_t length = 0;
    while (1) {
        char c = keyboard_get_char();
        if (c == '\r') {
            continue;
        }
        if (c == '\n') {
            console_put_char('\n');
            buffer[length] = '\0';
            return length;
        }
        if (c == '\b' || c == 127) {
            if (length > 0) {
                --length;
                console_backspace();
            }
            continue;
        }
        if (c >= 32 && c <= 126) {
            if (length + 1 < max_len) {
                buffer[length++] = c;
                console_put_char(c);
            }
        }
    }
}

static void shell_prompt(void)
{
    console_write("eduos> ");
}

static void execute_command(char *line)
{
    char *command = line;
    command = (char *)skip_spaces(command);
    if (!*command) {
        return;
    }

    char *args = command;
    while (*args && *args != ' ' && *args != '\t') {
        ++args;
    }

    if (*args) {
        *args++ = '\0';
    }
    args = (char *)skip_spaces(args);

    for (size_t i = 0; i < sizeof(commands) / sizeof(commands[0]); ++i) {
        if (str_compare(command, commands[i].name) == 0) {
            commands[i].handler(args);
            return;
        }
    }

    console_write("未知命令: ");
    console_write_line(command);
}

static void cmd_help(const char *args)
{
    UNUSED(args);
    console_write_line("可用命令:");
    for (size_t i = 0; i < sizeof(commands) / sizeof(commands[0]); ++i) {
        console_write("  ");
        console_write(commands[i].name);
        console_write(" - ");
        console_write_line(commands[i].description);
    }
}

static void cmd_info(const char *args)
{
    UNUSED(args);
    console_write_line("[info] EDU OS 简易教学内核");
    if (system_has_boot_info()) {
        const boot_info_t *info = system_boot_info();
        console_write(" 启动驱动: 0x");
        console_write_hex(info->boot_drive);
        console_write("  扇区: ");
        console_write_dec(info->kernel_sectors);
        console_put_char('\n');
    }
    uint32_t freq = pit_frequency_hz();
    console_write(" PIT 频率: ");
    console_write_dec(freq);
    console_write_line(" Hz");
}

static void cmd_clear(const char *args)
{
    UNUSED(args);
    console_clear();
}

static void cmd_echo(const char *args)
{
    console_write_line(skip_spaces(args));
}

static void cmd_ticks(const char *args)
{
    UNUSED(args);
    console_write("定时器节拍: ");
    console_write_dec((uint32_t)pit_ticks());
    console_put_char('\n');
}

static void cmd_sleep(const char *args)
{
    uint32_t ms;
    if (!parse_uint(args, &ms)) {
        console_write_line("用法: sleep <毫秒>");
        return;
    }
    uint32_t freq = pit_frequency_hz();
    if (freq == 0) {
        console_write_line("定时器尚未初始化");
        return;
    }
    uint64_t product = (uint64_t)ms * (uint64_t)freq;
    uint64_t ticks = div_u64_u32(product, 1000u);
    if (ticks == 0) {
        ticks = 1;
    }
    pit_sleep(ticks);
    console_write_line("完成");
}

static void cmd_mem(const char *args)
{
    UNUSED(args);
    console_write("内存总量: ");
    console_write_dec((uint32_t)memory_total());
    console_write_line(" bytes");
    console_write("已使用: ");
    console_write_dec((uint32_t)memory_used());
    console_write_line(" bytes");
    console_write("剩余: ");
    console_write_dec((uint32_t)memory_free());
    console_write_line(" bytes");
}

static void cmd_alloc(const char *args)
{
    uint32_t bytes;
    if (!parse_uint(args, &bytes) || bytes == 0) {
        console_write_line("用法: alloc <bytes>");
        return;
    }
    void *ptr = kmalloc(bytes);
    if (!ptr) {
        console_write_line("内存不足");
        return;
    }
    console_write("分配成功 地址: 0x");
    console_write_hex((uint32_t)(uintptr_t)ptr);
    console_put_char('\n');
}

static void cmd_uptime(const char *args)
{
    UNUSED(args);
    uint32_t freq = pit_frequency_hz();
    if (freq == 0) {
        console_write_line("定时器尚未初始化");
        return;
    }
    uint64_t ticks = pit_ticks();
    uint32_t seconds = (uint32_t)div_u64_u32(ticks, freq);
    console_write("运行时间: ");
    console_write_dec(seconds);
    console_write_line(" 秒");
}

static void cmd_halt(const char *args)
{
    UNUSED(args);
    console_write_line("系统停机");
    interrupts_disable();
    while (1) {
        __asm__ volatile ("hlt");
    }
}

void shell_run(void)
{
    console_write_line("欢迎进入 EDU OS 教学 Shell，输入 help 查看命令。");

    char buffer[INPUT_BUFFER_SIZE];
    while (1) {
        shell_prompt();
        size_t len = read_line(buffer, sizeof(buffer));
        if (len == 0) {
            continue;
        }
        execute_command(buffer);
    }
}
