ENTRY(kernel_entry)

PHDRS
{
    text PT_LOAD FLAGS(0x5); /* RX */
    data PT_LOAD FLAGS(0x6); /* RW */
}

SECTIONS
{
    . = 0x0010000;

    .text :
    {
        *(.multiboot)
        *(.text*)
    } :text

    .rodata :
    {
        *(.rodata*)
    } :text

    .data :
    {
        *(.data*)
    } :data

    .bss (NOLOAD) :
    {
        *(.bss*)
        *(COMMON)
    } :data
}
