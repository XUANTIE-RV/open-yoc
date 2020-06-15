/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

MEMORY
{
    RAM_SDRAM  : ORIGIN = 0x02000000 , LENGTH = 0x00280000   /* PRIM 2.5MB       */
    RAM_SDRAM2 : ORIGIN = 0x02280000 , LENGTH = 0x00170000   /* HEAP 1.5MB - 64K */
    SRAM       : ORIGIN = 0x023f0000 , LENGTH = 0x00010000   /* 64KB SD DMA      */
    RAM_DSP    : ORIGIN = 0x02400000 , LENGTH = 0x00400000   /* DSP 4MB          */
    SHADOW_SRAM : ORIGIN = 0x01000000 , LENGTH = 0x4000
    SYSTEM_SRAM : ORIGIN = 0x20000100 , LENGTH = 0x7F00
}

__min_heap_size = 0x200;
PROVIDE (__ram_end  = 0x023f0000);
PROVIDE (__heap_end = __ram_end);

REGION_ALIAS("REGION_TEXT",    RAM_SDRAM);
REGION_ALIAS("REGION_RODATA",  RAM_SDRAM);
REGION_ALIAS("REGION_DATA",    RAM_SDRAM);
REGION_ALIAS("REGION_BSS",     RAM_SDRAM);
REGION_ALIAS("REGION_SSRAM",   SRAM);
REGION_ALIAS("REGION_SRAM_TEXT", SHADOW_SRAM);
REGION_ALIAS("REGION_SRAM_DATA", SYSTEM_SRAM);

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
  *(EXCLUDE_FILE(*libsramsys.a *libappvad.a *liblpmmitvad.a) .rdata)
  *(EXCLUDE_FILE(*libsramsys.a *libappvad.a *liblpmmitvad.a) .rdata*)
  *(EXCLUDE_FILE(*libsramsys.a *libappvad.a *liblpmmitvad.a) .rdata1)
  *(EXCLUDE_FILE(*libsramsys.a *libappvad.a *liblpmmitvad.a) .rdata.*)
  *(EXCLUDE_FILE(*libsramsys.a *libappvad.a *liblpmmitvad.a) .rodata)
  *(EXCLUDE_FILE(*libsramsys.a *libappvad.a *liblpmmitvad.a) .rodata1)
  *(EXCLUDE_FILE(*libsramsys.a *libappvad.a *liblpmmitvad.a) .rodata*)
  *(EXCLUDE_FILE(*libsramsys.a *libappvad.a *liblpmmitvad.a) .rodata.*)
  *(EXCLUDE_FILE(*libsramsys.a *libappvad.a *liblpmmitvad.a) .rodata.str1.4)
   _sinitcall = .;
   KEEP(*(.initcall))
   _einitcall = .;
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
 .sram_shadow : {
  . = ALIGN(0x4) ;
  __sram_text_start__ = . ;
  *(.sram.vectors)
  *(.sram.text)
  *(.sram.text2)
  *libsramsys.a:(.rodata* .rdata*)
  *libappvad.a:(.rodata* .rdata*)
  *liblpmmitvad.a:(.rodata* .rdata*)
  __sram_text_end__ = . ;
 } > REGION_SRAM_TEXT AT >  REGION_DATA
 .sram_system : {
  . = ALIGN(0x4) ;
  __sram_data_start__ = . ;
  *(.sram.data)
  *(.sram.data)
  *(.sram.bss)
  __sram_data_end__ = . ;
 } > REGION_SRAM_DATA AT >  REGION_DATA
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
 } > RAM_SDRAM2
  .sys_sram (NOLOAD) : {
  *(.sys_sram)
 } > REGION_SSRAM
}
