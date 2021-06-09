/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     gcc_csky.h
 * @brief    csky linker file for PANGU
 * @version  V1.0
 * @date     18. Mar 2019
 ******************************************************************************/
MEMORY
{
    SPIFLASH : ORIGIN = 0x80A0000  , LENGTH = 0x800000 /* 8M */
    ITCM0   : ORIGIN = 0xFFE0000  , LENGTH = 0x7FFF    /* 32KB*/
    ITCM1   : ORIGIN = 0xFFE8000  , LENGTH = 0x7FFF    /* 32KB*/
    ITCM2   : ORIGIN = 0xFFF0000  , LENGTH = 0x7FFF    /* 32KB*/
    ITCM3   : ORIGIN = 0xFFF8000  , LENGTH = 0xFFFF    /* 32KB*/
    DTCM0   : ORIGIN = 0x10000000 , LENGTH = 0x7FFF    /* 32KB*/
    DTCM1   : ORIGIN = 0x10008000 , LENGTH = 0x7FFF    /* 32KB*/
    DTCM2   : ORIGIN = 0x10010000 , LENGTH = 0x7FFF    /* 32KB*/
    DTCM3   : ORIGIN = 0x10018000 , LENGTH = 0xFFFF    /* 32KB*/
    SDRAM  : ORIGIN = 0x18000000 , LENGTH = 0x300000
}

/*
__min_heap_size = 0x200;
PROVIDE (__ram_end  = 0x28014000);
PROVIDE (__heap_end = __ram_end);

REGION_ALIAS("REGION_TEXT",    CPU0ISRAM);
REGION_ALIAS("REGION_RODATA",  CPU0ISRAM);
REGION_ALIAS("REGION_DATA",    CPU0DSRAM);
REGION_ALIAS("REGION_BSS",     CPU0DSRAM);
REGION_ALIAS("REGION_SSRAM",   SHAREDSRAM);
*/

/*
__min_heap_size = 0x200;
PROVIDE (__ram_end  = 0x10010000);
PROVIDE (__heap_end = __ram_end);
REGION_ALIAS("REGION_TEXT",    ITCM0);
REGION_ALIAS("REGION_RODATA",  ITCM0);
REGION_ALIAS("REGION_DATA",    DTCM0);
REGION_ALIAS("REGION_BSS",     DTCM0);
REGION_ALIAS("REGION_SSRAM",   DTCM0);
*/

/*
__min_heap_size = 0x200;
PROVIDE (__ram_end  = 0x30800000);
PROVIDE (__heap_end = __ram_end);
REGION_ALIAS("REGION_TEXT",    SPIFLASH);
REGION_ALIAS("REGION_RODATA",  SPIFLASH);
REGION_ALIAS("REGION_DATA",    SDRAM);
REGION_ALIAS("REGION_BSS",     SDRAM);
REGION_ALIAS("REGION_SSRAM",   SDRAM);
*/
__min_heap_size = 0x200;
PROVIDE (__ram_end  = 0x18300000);
PROVIDE (__heap_end = __ram_end);
REGION_ALIAS("REGION_TEXT",    SPIFLASH);
REGION_ALIAS("REGION_RODATA",  SPIFLASH);
REGION_ALIAS("REGION_DATA",    SDRAM);
REGION_ALIAS("REGION_BSS",     SDRAM);
REGION_ALIAS("REGION_SSRAM",   SDRAM);

ENTRY(Reset_Handler)
SECTIONS
{
 .text :  {
  . = ALIGN(0x4) ;
  KEEP(*startup.o(*.text*))
  KEEP(*system.o(*.text*))
  __stext = . ;
  EXCLUDE_FILE_USER_DEFINE
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
  __flash_code_end__ = . ;
  __etext = . ;
 } > REGION_TEXT
 .eh_frame_hdr : {
  *(.eh_frame_hdr)
 } > REGION_TEXT
 .eh_frame : ONLY_IF_RO {
  KEEP (*(.eh_frame))
 } > REGION_TEXT
 .gcc_except_table : ONLY_IF_RO {
  *(.gcc_except_table .gcc_except_table.*)
 } > REGION_TEXT
 .rodata :  {
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
 } > REGION_RODATA
 .data :  {
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
 } > REGION_DATA AT > REGION_RODATA
 .eh_frame : ONLY_IF_RW {
  KEEP (*(.eh_frame))
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
  USER_DEFINE_REGION
  __ram_code_end__ = .;
  . = ALIGN(0x4) ;
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
  __end = . ;
  end = . ;
  __bss_end__ = .;
 } > REGION_BSS AT > REGION_BSS
 ._user_heap : {
  . = ALIGN(0x4) ;
  __heap_start = .;
  . += __min_heap_size;
  . = ALIGN(0x4) ;
 } > REGION_BSS AT > REGION_BSS

 .sys_sram (NOLOAD) : {
    *(.sys_sram)
 } > REGION_SSRAM
 .tcm1 (NOLOAD) : {
    *(.tcm1)
 } >DTCM1
}
