
SECTIONS
{
    .boot_ram 0x401000:
    {
        KEEP(*(.text.boot_ram_start))
        *(.xip_banned .text* .rodata* .sdata* .data* .sbss* .bss* )
    }
    .got :
    {
        *(.got)
    }
    .got.plt :
    {
        *(.got.plt)
    }
}