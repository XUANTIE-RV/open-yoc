/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     gcc_csky.h
 * @brief    csky linker file for PANGU
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/
MEMORY
{
    QSPIFLASH : ORIGIN = 0x8000000 , LENGTH = 0x800000

    TCM0      : ORIGIN = 0x80010000, LENGTH = 0x20000    /* CPU1 I-SRAM   64KB */

    TCM1_CHUNK_0   : ORIGIN = 0x80020000, LENGTH = 0x1000    /* CHUNK BUFFER 0 4KB */
    TCM1_FAST_HEAP : ORIGIN = 0x80021000, LENGTH = 0xe000    /* FAST HEAP     56KB */
    TCM1_CHUNK_1   : ORIGIN = 0x8002F000, LENGTH = 0x1000    /* CHUNK BUFFER 1 4KB */



	  TCM1      : ORIGIN = 0x80020000, LENGTH = 0x10000   /* OTHERS        64KB */

    SDRAM_CODE : ORIGIN = 0x18600000, LENGTH = 0x200000  /* SDRAM          2MB */
}

__min_heap_size = 0x200;
PROVIDE (__ram_end  = 0x18800000);
PROVIDE (__heap_end = __ram_end);

REGION_ALIAS("REGION_TEXT"          , SDRAM_CODE);
REGION_ALIAS("REGION_RODATA"        , SDRAM_CODE);
REGION_ALIAS("REGION_DATA"          , SDRAM_CODE);
REGION_ALIAS("REGION_BSS"           , SDRAM_CODE);
REGION_ALIAS("REGION_CHUNK_BUFFER_0", TCM1_CHUNK_0  );
REGION_ALIAS("REGION_CHUNK_BUFFER_1", TCM1_CHUNK_1  );
REGION_ALIAS("REGION_FAST_HEAP"     , TCM1_FAST_HEAP);

ENTRY(Reset_Handler)
SECTIONS
{
 .text :  {
  . = ALIGN(0x4) ;
  KEEP(*startup.o(.text))
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
  *(.init)
  *(.fini)
  . = ALIGN (4) ;
  PROVIDE(__ctbp = .);
  *(.call_table_data)
  *(.call_table_text)
  . = ALIGN(0x10) ;
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
 } > REGION_DATA
 .eh_frame : ONLY_IF_RW {
  KEEP (*(.eh_frame))
 } > REGION_DATA
 .gcc_except_table : ONLY_IF_RW {
  *(.gcc_except_table .gcc_except_table.*)
  __edata = .;
  __data_end__ = .;
 } > REGION_DATA

 .sdram : {
   . = ALIGN(4) ;
   *(.sdram)
   __data_end__ = .;
 } > REGION_DATA

 .chunk_buffer_0 : {
   . = ALIGN(0x4) ;
   __chunk_buffer0_start = . ;
   *(.chunk_buffer_0) ;
   . = ALIGN(0x4) ;
   __chunk_buffer0_end = . ;
 } > REGION_CHUNK_BUFFER_0 AT > REGION_DATA 

 .chunk_buffer_1 : {
   . = ALIGN(0x4) ;
   __chunk_buffer1_start = . ;
   *(.chunk_buffer_1) ;
   . = ALIGN(0x4) ;
   __chunk_buffer1_end = . ;
 } > REGION_CHUNK_BUFFER_1 AT > REGION_DATA 

 .fast_heap : {
   . = ALIGN(0x4) ;
   __fast_heap_start = . ;
   *(.fast_heap) ;
   . = ALIGN(0x4) ;
   __fast_heap_end = . ;
 } > REGION_FAST_HEAP AT > REGION_DATA 

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
 
}
