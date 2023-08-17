/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     gcc_csky.ld
 * @brief    csky linker file
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/
MEMORY
{
    SPIFLASH : ORIGIN = 0x18017000 , LENGTH = 0xF0000   /* SPI-FLASH 960KB*/
    DSRAM    : ORIGIN = 0x20000000 , LENGTH = 0x30000   /* on-chip DSRAM 192KB*/
    SRAM     : ORIGIN = 0x08000000 , LENGTH = 0x10000   /* on-chip SRAM 64KB*/ 
}

__min_heap_size = 0x200;
PROVIDE (__ram_end  = 0x20030000);
PROVIDE (__heap_end = __ram_end);

REGION_ALIAS("REGION_TEXT",    SPIFLASH);
REGION_ALIAS("REGION_RODATA",  SPIFLASH);
REGION_ALIAS("REGION_DATA",    SRAM);
REGION_ALIAS("REGION_BSS",     SRAM);
REGION_ALIAS("REGION_HEAP",    DSRAM);

ENTRY(Reset_Handler)
SECTIONS
{
 .text : {
  . = ALIGN(0x4) ;
  *startup.o(*.text)
  __stext = . ;
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
 } > REGION_TEXT
 .gcc_except_table : ONLY_IF_RO {
  *(.gcc_except_table .gcc_except_table.*)
 } > REGION_TEXT
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
  *(.srodata*)
  . = ALIGN(0x4) ;
  __init_array_start = .;
  __ctors_start__ = .;
  KEEP (*(SORT(.init_array.*)))
  KEEP (*(.init_array))
  __init_array_end = .;
  __ctors_end__ = .;

  __fini_array_start = .;
  __dtors_start__ = .;
  KEEP (*(SORT(.fini_array.*)))
  KEEP (*(.fini_array))
  __fini_array_end = .;
  __dtors_end__ = .;

  __ctor_start__ = .;
  KEEP (*(SORT(.ctors.*)))
  KEEP (*(.ctors))
  __ctor_end__ = .;
  KEEP (*(SORT(.dtors.*)))
  KEEP (*(.dtors))
  __dtor_end__ = .;
  . = ALIGN(0x4) ;
/************** added drivers **************/
  _cli_region_begin = .;
  KEEP(*(CliRegion))
  . = ALIGN(4);
  _cli_region_end = .;

  __core_driver_start__ = .;
  KEEP(*(.core_driver_entry))
  . = ALIGN(4);
  __core_driver_end__ = .;

  __bus_driver_start__ = .;
  KEEP(*(*.bus_driver_entry))
  __bus_driver_end__ = .;

  __early_driver_start__ = .;
  KEEP(*(*.early_driver_entry))
  __early_driver_end__ = .;

  __vfs_driver_start__ = .;
  KEEP(*(*.vfs_driver_entry))
  __vfs_driver_end__ = .;

  __level0_driver_start__ = .;
  KEEP(*(*.level0_driver_entry))
  __level0_driver_end__ = .;

  __level1_driver_start__ = .;
  KEEP(*(*.level1_driver_entry))
  __level1_driver_end__ = .;

  __level2_driver_start__ = .;
  KEEP(*(*.level2_driver_entry))
  __level2_driver_end__ = .;

  __level3_driver_start__ = .;
  KEEP(*(*.level3_driver_entry))
  __level3_driver_end__ = .;

  __post_driver_start__ = .;
  KEEP(*(*.post_driver_entry))
  __post_driver_end__ = .;
/************** end of drivers *********/
  . = ALIGN(0x4) ;
  __erodata = .;
  __rodata_end__ = .;
 } > REGION_RODATA
 .data : {
  . = ALIGN(0x4) ;
  __sdata = . ;
  __data_start__ = . ;
  data_start = . ;
  KEEP(*startup.o(*.vectors*))
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
  __global_pointer$ = .;
  *(.sdata)
  *(.sdata.*)
  *(.sdata2.*)
  *(.gnu.linkonce.s.*)
  *(__libc_atexit)
  *(__libc_subinit)
  *(__libc_subfreeres)
  *(.note.ABI-tag)
  . = ALIGN(0x4) ;
  __edata = .;
  __data_end__ = .;
  . = ALIGN(0x4) ;
 } > REGION_DATA AT > REGION_RODATA
 .gcc_except_table : ONLY_IF_RW {
  *(.gcc_except_table .gcc_except_table.*)
  __edata = .;
  __data_end__ = .;
 } > REGION_DATA AT > REGION_RODATA
 ._ram_code : {
  . = ALIGN(0x4) ;
  __ram_code_start__ = .;
  *(.ram.code*)
  . = ALIGN(0x4) ;
  __ram_code_end__ = .;
  } > REGION_DATA AT > REGION_RODATA
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
  __bss_end__ = .;
  __end = . ;
  end = . ;
 } > REGION_BSS AT > REGION_BSS
 ._user_heap_stack (NOLOAD) : {
  . = ALIGN(0x4) ;
  *(.stack*)
  . = ALIGN(0x4) ;
  __heap_start = .;
  . += __min_heap_size;
  . = ALIGN(0x4) ;
 } > REGION_HEAP
}
