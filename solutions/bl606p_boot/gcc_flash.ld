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
    FLASH (rxai!w)  : ORIGIN = 0x58000000, LENGTH = 128K
    PSRAM(rx)       : ORIGIN = 0x54000000, LENGTH = 128K
}

__min_heap_size = 0x200;
BOOT2_PT_ADDR = 0x22047E00;

PROVIDE (__ram_end  = ORIGIN(PSRAM) + LENGTH(PSRAM));
PROVIDE (__heap_end = __ram_end);
PROVIDE ( __boot2_pt_addr_src = BOOT2_PT_ADDR );

REGION_ALIAS("REGION_USERDEFINE", FLASH);
REGION_ALIAS("REGION_TEXT",    FLASH);
REGION_ALIAS("REGION_RODATA",  FLASH);
REGION_ALIAS("REGION_DATA",    PSRAM);
REGION_ALIAS("REGION_BSS",     PSRAM);

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
 } > REGION_TEXT AT > REGION_USERDEFINE
 .eh_frame : ONLY_IF_RO {
  KEEP (*(.eh_frame))
 } > REGION_TEXT AT > REGION_USERDEFINE
 .gcc_except_table : ONLY_IF_RO {
  *(.gcc_except_table .gcc_except_table.*)
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
  __erodata = .;
  __rodata_end__ = .;
 } > REGION_RODATA AT > REGION_USERDEFINE
 .data : {
  . = ALIGN(0x4) ;
  __sdata = . ;
  __data_start__ = . ;
  data_start = . ;
/*  KEEP(*startup.o(*.vectors*))*/
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
  
  *(.tcm_code*)
  *(.tcm_const*)
  *(.sclock_rlt_code*)
  *(.sclock_rlt_const*)
    
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
  __edata = .;
  __data_end__ = .;
  . = ALIGN(0x4) ;
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
  __ram_code_end__ = .;
  . = ALIGN(0x4) ;
 } > REGION_DATA AT > REGION_USERDEFINE
 .boot2_bss (NOLOAD) :
 {
     PROVIDE ( __boot2_pt_addr_start = . );
     *(.bss.boot2_partition_table)
     PROVIDE ( __boot2_pt_addr_end   = . );
 } > REGION_BSS
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
 } > REGION_BSS 
 ._user_heap : {
  . = ALIGN(0x8) ;
  __heap_start = .;
  . += __min_heap_size;
  . = ALIGN(0x8) ;
 } > REGION_BSS
}
