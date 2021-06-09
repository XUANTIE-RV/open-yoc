
SECTIONS
{
    .boot_ram 0x0:
    {
        KEEP(*(.rodata.vector))
        *(.xip_banned .text* .rodata* .data* )
    }
    .bss (NOLOAD):
    {
        *(.bss* )
    }
}
