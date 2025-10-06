# EDU OS —— 教学用途的迷你操作系统

EDU OS 是一个面向学习者编写的 32 位 x86 教学操作系统示例。它使用自制的引导扇区启动，进入保护模式后加载一个用 C 语言编写的内核，演示了终端输出、可编程定时器 (PIT)、键盘中断、简单内存分配器以及一个交互式 shell 等基础功能。代码刻意保持紧凑，方便阅读与实验。

## 功能概览
- 自制 16 位引导扇区，启用 A20 线、加载内核并切换到保护模式。
- 32 位内核入口，清理 `.bss`、初始化 GDT/IDT/PIC。
- VGA 文本模式输出与硬件光标控制。
- 可编程中断控制器 (PIC) 重新映射、统一的中断分发框架。
- PIT 定时器驱动与节拍计数，支持软等待 (`sleep` 命令)。
- PS/2 键盘驱动，带缓冲区与基本按键映射、Shift/CapsLock 处理。
- 1 MiB 简易堆内存（线性 bump allocator），提供 `kmalloc` 示例。
- 内置交互式 Shell，提供 `help / info / mem / sleep / alloc / uptime / halt` 等命令。

## 目录结构
```
boot/           引导扇区源码（16 位）
include/        内核公共头文件
kernel/         内核 C/汇编源码
scripts/        构建辅助脚本
linker.ld       内核链接脚本
Makefile        构建入口
```

## 构建与运行
依赖：`gcc` (支持 `-m32`)、`ld`、`objcopy`、`python3`、`make`。运行需要 `qemu-system-i386` 或其他可加载原始磁盘映像的虚拟机。

```bash
make                # 生成 build/os-image.bin
make clean          # 清理中间文件
```

运行（若本机已安装 QEMU）：
```bash
qemu-system-i386 -drive format=raw,file=build/os-image.bin -curses
```
> 本开发环境未预装 QEMU，上述命令将在目标机器上执行。

## Shell 命令速览
| 命令       | 说明                                      |
|------------|-------------------------------------------|
| `help`     | 列出所有可用命令                          |
| `info`     | 显示引导信息与定时器频率                  |
| `mem`      | 查看堆内存总量/已用/剩余                  |
| `ticks`    | 当前 PIT 节拍计数                         |
| `sleep X`  | 休眠 X 毫秒                               |
| `alloc X`  | 向教学堆申请 X 字节并返回地址             |
| `echo ...` | 原样回显文本                              |
| `uptime`   | 运行时间（秒）                            |
| `clear`    | 清空屏幕                                  |
| `halt`     | 停机并进入 HLT 循环                       |

## 代码阅读指引
1. `boot/boot.S`：BIOS 阶段的所有操作入口，包含 A20、GDT、保护模式切换。
2. `kernel/entry.S`：32 位入口，清理 `.bss` 并跳入 C 代码。
3. `kernel/main.c`：内核初始化顺序与 Shell 启动。
4. `kernel/interrupts.c` + `kernel/isr.S`：中断分发与异常处理。
5. `kernel/console.c`：VGA 文本操作与滚屏逻辑。
6. `kernel/keyboard.c`、`kernel/pit.c`：外设驱动示例。
7. `kernel/memory.c`：教学用线性分配器实现。

欢迎在此基础上继续扩展，例如加入任务切换、分页管理、文件系统等更深入的实验。
