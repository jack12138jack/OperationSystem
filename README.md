# EduOS

EduOS 是一个面向教学的极简操作系统内核，展示了从 BIOS 启动到进入保护模式、初始化终端与键盘、实现教学命令行的完整流程。该项目可以在 QEMU 中运行，适合用于讲解低层系统编程的核心概念。

## 功能亮点

- **自制引导扇区**：`boot.asm` 包含 16 位实模式引导程序，负责开启 A20、加载内核并切换到 32 位保护模式。
- **基础内核框架**：内核采用 C 与少量汇编编写，通过自定义链接脚本加载到 0x0010000，并在 VGA 文本模式下输出信息。
- **教学命令行**：提供 `help`、`about`、`lessons`、`lesson <n>`、`memory`、`clear`、`reboot` 等命令，帮助学习者逐步了解操作系统核心概念。
- **键盘驱动示例**：`keyboard.c` 展示了如何通过端口读取键盘扫描码并进行转码，支持 Shift/Caps Lock、退格与回车。
- **可扩展的课程引导**：`command.c` 中的教学条目给出扩展练习方向，便于继续实现中断、调度、文件系统等模块。

## 目录结构

```
.
├── Makefile            # 构建规则，生成裸机镜像 `os-image.bin`
├── README.md
├── include/            # 公共头文件
├── src/
│   ├── boot.asm        # 引导扇区
│   ├── linker.ld       # 链接脚本
│   └── kernel/
│       ├── command.c   # 教学命令行与课程内容
│       ├── keyboard.c  # 键盘驱动
│       ├── kernel.c    # 内核主入口
│       ├── kernel_entry.asm # 32 位入口桩
│       ├── terminal.c  # VGA 文本终端
│       └── util.c      # 简易字符串/工具函数
└── os-image.bin        # 运行后生成的启动镜像（构建时产生）
```

## 构建与运行

1. 构建系统镜像：

   ```bash
   make
   ```

   该命令将编译引导扇区与内核，并生成 `os-image.bin`。

2. 在 QEMU 中运行：

   ```bash
   make run
   ```

   或手动执行：

   ```bash
   qemu-system-i386 -drive format=raw,file=os-image.bin -serial stdio
   ```

   启动后屏幕会显示欢迎信息，并进入 `EduOS>` 提示符。按 `help` 查看可用命令。

## 学习建议

- 阅读 `boot.asm` 了解 BIOS 调用、磁盘读写与 GDT 设置。
- 在 `terminal.c` 中尝试更改颜色或实现更复杂的滚屏逻辑。
- 根据 `command.c` 的提示，逐步添加中断描述符表、时钟中断与调度器等模块。
- 若要扩展文件系统或用户态支持，可以从实现内存分页和简单的任务结构体开始。

## 清理

```bash
make clean
```

将移除构建过程中生成的中间文件和镜像。
