OUTPUT_FORMAT("elf32-i386")
ENTRY(_start)
SECTIONS
{
  PROVIDE(kernel_start = 0x0010000);
  . = 0x0010000;

  .text ALIGN(16) :
  {
    *(.text*)
  }

  .rodata ALIGN(16) :
  {
    *(.rodata*)
  }

  .data ALIGN(16) :
  {
    *(.data*)
  }

  .bss ALIGN(16) :
  {
    __bss_start = .;
    *(.bss*)
    *(COMMON)
    __bss_end = .;
  }

  PROVIDE(kernel_end = .);
  PROVIDE(__bss_start = __bss_start);
  PROVIDE(__bss_end = __bss_end);
}
