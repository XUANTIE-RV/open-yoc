/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     gcc_csky_flash.ld
 * @brief    csky linker file for SC5654A
 * @version  V1.0
 * @date     02. April 2019
 ******************************************************************************/

MEMORY
{
    SHADOW_SRAM : ORIGIN = 0x01000000 , LENGTH = 0x4000
    SYSTEM_SRAM : ORIGIN = 0x20000000 , LENGTH = 0x8000
}

__min_heap_size = 0x200;
PROVIDE (__ram_end  = 0x20008000);
PROVIDE (__heap_end = __ram_end);

REGION_ALIAS("REGION_TEXT",    SYSTEM_SRAM);
REGION_ALIAS("REGION_RODATA",  SYSTEM_SRAM);
REGION_ALIAS("REGION_DATA",    SYSTEM_SRAM);
REGION_ALIAS("REGION_BSS",     SYSTEM_SRAM);
REGION_ALIAS("REGION_SSRAM",   SYSTEM_SRAM);


ENTRY(Reset_Handler)
SECTIONS
{
 .text : {
  . = ALIGN(0x4) ;
  __stext = . ;
  *startup.o(*.text)
  *(.text)
  *(.text*)
  *(.text.*)
  *(.gnu.warning)
  *(.stub)
  *(.gnu.linkonce.t*)
  *(.glue_7t)
  *(.glue_7)
  *(.jcr)
  *(.init)
  *(.fini)
  . = ALIGN (4) ;
  PROVIDE(__ctbp = .);
  *(.call_table_data)
  *(.call_table_text)
  . = ALIGN(0x10) ;
  __etext = . ;
 } > REGION_TEXT
 .rodata : {
  . = ALIGN(0x4) ;
  __srodata = .;
   _sinitcall = .;
   *(.rodata*)
   *(.rodata.*)
   KEEP(*(.initcall))
   _einitcall = .;
  . = ALIGN(0x4) ;
  __erodata = .;
 } > REGION_RODATA
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
  _net_buf_pool_list = .;
  KEEP(*(SORT_BY_NAME("._net_buf_pool.static.*")))
  . = ALIGN(0x4) ;
  __edata = .;
  __data_end__ = .;
 } > REGION_DATA
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
 } > REGION_BSS
 ._user_heap : {
  . = ALIGN(0x4) ;
  __heap_start = .;
  . += __min_heap_size;
  . = ALIGN(0x4) ;
 } > SYSTEM_SRAM
  .sys_sram (NOLOAD) : {
  *(.sys_sram)
 } > REGION_SSRAM
}
