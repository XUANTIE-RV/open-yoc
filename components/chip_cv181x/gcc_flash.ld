/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     gcc_csky.ld
 * @brief    csky linker file
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/
INCLUDE cvi_memory.ld

/* default DRAM 64M-256K */
MEMORY
{
    DRAM   : ORIGIN = 0x80040000, LENGTH = DRAM_LENGTH
}

__min_heap_size = 0x200;
PROVIDE (__ram_end  = ORIGIN(DRAM) + LENGTH(DRAM));
PROVIDE (__heap_end = __ram_end);

REGION_ALIAS("REGION_TEXT",         DRAM);
REGION_ALIAS("REGION_RODATA",       DRAM);
REGION_ALIAS("REGION_DATA",         DRAM);
REGION_ALIAS("REGION_BSS",          DRAM);

ENTRY(Reset_Handler)
SECTIONS
{
 .text : {
  . = ALIGN(0x4) ;
  __text_start = .;
  __stext = . ;
  KEEP(*startup.o(*.text))
  KEEP(*startup.o(*.vectors))
  KEEP(*vectors.o(*.text))
  KEEP(*port_s.o(*.text))
  *(.text*)
  *(.gnu.warning)
  *(.stub)
  *(.gnu.linkonce.t*)
  *(.glue_7t)
  *(.glue_7)
  *(.jcr)
  KEEP (*(.init))
  KEEP (*(.fini))
  . = ALIGN(0x8) ;
  PROVIDE(__ctbp = .);
  *(.call_table_data)
  *(.call_table_text)
  __etext = . ;
  __text_end = .;
 } > REGION_TEXT
 .rodata ALIGN(0x1000): {
  __rodata_start = .;
  __srodata = .;
  *(.rdata*)
  *(.rodata*)
  *(.srodata*)
  . = ALIGN(0x8) ;

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
  . = ALIGN(0x8) ;

    /************** added drivers **************/
    _cli_region_begin = .;
    KEEP(*(CliRegion))
    . = ALIGN(8);
    _cli_region_end = .;

    __core_driver_start__ = .;
    KEEP(*(.core_driver_entry))
    . = ALIGN(8);
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
  . = ALIGN(0x8);
  __erodata = .;
  __rodata_end__ = .;
  __rodata_end = .;
 } > REGION_RODATA
 .data ALIGN(0x1000): {
  __data_start = .;
  __sdata = . ;
  __data_start__ = . ;
  data_start = . ;
  *(.got.plt)
  *(.got)
  *(.gnu.linkonce.r*)
  *(.data*)
  *(.gnu.linkonce.d*)
  *(.gcc_except_table)
  *(.gcc_except_table*)
  __start_init_call = .;
  *(.initcall.init)
  __stop_init_call = .;
  __start_cmd = .;
  *(.bootloaddata.cmd)
  . = ALIGN(8) ;
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
 . = ALIGN(0x8);
  __edata = .;
  __data_end__ = .;
  __data_end = .;
 } > REGION_DATA

 .bss ALIGN(0x1000): {
  __bss_start = .;
  __sbss = ALIGN(0x8) ;
  __bss_start__ = . ;
  *(.dynsbss)
  *(.sbss)
  *(.sbss.*)
  *(.scommon)
  *(.dynbss)
  *(.bss)
  *(.bss.*)
  *(COMMON)
  . = ALIGN(0x8) ;
  __ebss = . ;
  __bss_end__ = .;
  __end = . ;
  end = . ;
 __bss_end = .;
 } > REGION_BSS
 ._user_heap ALIGN(0x1000) (NOLOAD) : {
  __heap_start0 = .;
  *(.stack*)
  . = ALIGN(0x8) ;
  __heap_start = .;
  . = ALIGN(0x8) ;
  . += __min_heap_size;
  . = ALIGN(0x8) ;
 } > REGION_BSS
 . = ALIGN(16);
 _mmu_text_start        = __text_start;
 _mmu_text_end          = __text_end;
 _mmu_text_len          = _mmu_text_end - _mmu_text_start;

 _mmu_rodata_start      = __rodata_start;
 _mmu_rodata_end        = __rodata_end;
 _mmu_rodata_len        = _mmu_rodata_end - _mmu_rodata_start;

 _mmu_data_start        = __data_start;
 _mmu_data_end          = __data_end;
 _mmu_data_len          = _mmu_data_end - _mmu_data_start;

 _mmu_bss_start         = __bss_start;
 _mmu_bss_end           = __bss_end;
 _mmu_bss_len           = _mmu_bss_end - _mmu_bss_start;

 _mmu_heap_start        = __heap_start0;
 _mmu_heap_end          = __heap_end;
 _mmu_heap_len          = _mmu_heap_end - _mmu_heap_start;
}
