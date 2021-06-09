MEMORY
{
    SPIFLASH : ORIGIN = 0x8001000 , LENGTH = 0x20000
    DTCM : ORIGIN = 0X0FFE0000 , LENGTH = 0x40000
}
__min_heap_size = 0x200;
PROVIDE (__ram_end = 0x10020000);
PROVIDE (__heap_end = __ram_end);
REGION_ALIAS("REGION_USERDEFINE", SPIFLASH);
REGION_ALIAS("REGION_TEXT", DTCM);
REGION_ALIAS("REGION_RODATA", DTCM);
REGION_ALIAS("REGION_DATA", DTCM);
REGION_ALIAS("REGION_BSS", DTCM);
REGION_ALIAS("REGION_SSRAM", DTCM);
ENTRY(Reset_Handler)
SECTIONS
{
 ._flash_code : {
  . = ALIGN(0x4) ;
  KEEP(*startup.o(*.text*))
  KEEP(*system.o(*.text*))
  __flash_code_start__ = .;
  *(.flash.code*)
  . = ALIGN(0x4) ;
  __flash_code_end__ = .;
  } > REGION_USERDEFINE
 .text : {
  . = ALIGN(0x4) ;
  __stext = . ;
  *startup.o(*.text*)
  *system.o(*.text*)
  *(.text)
  *(.text*)
  *(.text.*)
  *(.gnu.warning)
  *(.stub)
  *(.gnu.linkonce.t*)
  *(.glue_7t)
  *(.glue_7)
  *(.jcr)
  KEEP (*(.init))
  KEEP (*(.fini))
  . = ALIGN (4) ;
  PROVIDE(__ctbp = .);
  *(.call_table_data)
  *(.call_table_text)
  . = ALIGN(0x10) ;
  __etext = . ;
 } > REGION_TEXT AT > REGION_USERDEFINE
 .eh_frame_hdr : {
  *(.eh_frame_hdr)
  . = ALIGN(4) ;
  __etext = . ;
 } > REGION_TEXT
 .eh_frame : ONLY_IF_RO {
  KEEP (*(.eh_frame))
  . = ALIGN(4) ;
  __etext = . ;
 } > REGION_TEXT AT > REGION_USERDEFINE
 .gcc_except_table : ONLY_IF_RO {
  *(.gcc_except_table .gcc_except_table.*)
  . = ALIGN(4) ;
  __etext = . ;
 } > REGION_TEXT AT > REGION_USERDEFINE
 .rodata : {
  . = ALIGN(0x4) ;
  __srodata = .;
  *(.rdata)
  *(.rdata*)
  *(.rdata1)
  *(.rdata.*)
  *(.rodata)
  *(.rodata1)
  *(.rodata*)
  *(.rodata.*)
  *(.rodata.str1.4)
  . = ALIGN(0x4) ;
  __ctor_start__ = .;
  KEEP (*(SORT(.ctors.*)))
  KEEP (*(.ctors))
  __ctor_end__ = .;
  KEEP (*(SORT(.dtors.*)))
  KEEP (*(.dtors))
  __dtor_end__ = .;
  . = ALIGN(0x4) ;
  __erodata = .;
 } > REGION_RODATA AT > REGION_USERDEFINE
 .data : {
  . = ALIGN(0x4) ;
  __sdata = . ;
  __data_start__ = . ;
  data_start = . ;
  KEEP(*startup.o(.vectors))
  *(.got.plt)
  *(.got)
  *(.gnu.linkonce.r*)
  *(.data)
  *(.data*)
  *(.data1)
  *(.data.*)
  *(.gnu.linkonce.d*)
  *(.data1)
  *(.gcc_except_table)
  *(.gcc_except_table*)
  __start_init_call = .;
  *(.initcall.init)
  __stop_init_call = .;
  __start_cmd = .;
  *(.bootloaddata.cmd)
  . = ALIGN(4) ;
  __stop_cmd = .;
  *(.sdata)
  *(.sdata.*)
  *(.gnu.linkonce.s.*)
  *(__libc_atexit)
  *(__libc_subinit)
  *(__libc_subfreeres)
  *(.note.ABI-tag)
  . = ALIGN(0x4) ;
  __edata = .;
  __data_end__ = .;
 } > REGION_DATA AT > REGION_USERDEFINE
 .eh_frame : ONLY_IF_RW {
  KEEP (*(.eh_frame))
 } > REGION_DATA AT > REGION_USERDEFINE
 .gcc_except_table : ONLY_IF_RW {
  *(.gcc_except_table .gcc_except_table.*)
  __edata = .;
  __data_end__ = .;
 } > REGION_DATA AT > REGION_USERDEFINE
 ._ram_code : {
  . = ALIGN(0x4) ;
  __ram_code_start__ = .;
  *(.ram.code*)
  . = ALIGN(0x4) ;
  __ram_code_end__ = .;
  } > REGION_DATA AT > REGION_USERDEFINE
 .bss : {
  . = ALIGN(0x4) ;
  __sbss = ALIGN(0x4) ;
  __bss_start__ = . ;
  *(.dynsbss)
  *(.sbss)
  *(.sbss.*)
  *(.scommon)
  *(.dynbss)
  *(.bss)
  *(.bss.*)
  *(COMMON)
  . = ALIGN(0x4) ;
  __ebss = . ;
  __end = . ;
  end = . ;
  __bss_end__ = .;
 } > REGION_BSS AT > REGION_BSS
._user_heap_stack (NOLOAD) : {
  . = ALIGN(0x4) ;
  *(.stack*)
  . = ALIGN(0x4) ;
  __heap_start = .;
  . += __min_heap_size;
  . = ALIGN(0x4) ;
 } > REGION_BSS AT > REGION_BSS
 .sys_sram (NOLOAD) : {
    *(.sys_sram)
 } > REGION_SSRAM
}
