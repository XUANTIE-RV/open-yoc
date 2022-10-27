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
    PSRAM(rx)       : ORIGIN = 0x54C00000, LENGTH = 4M
    L2SRAM(!rx)     : ORIGIN = 0x3EF80000, LENGTH = 512K
}

__min_heap_size = 0x200;
PROVIDE (__ram_end  = ORIGIN(PSRAM)+LENGTH(PSRAM));
PROVIDE (__heap_end = __ram_end);
PROVIDE (__heap2_end = ORIGIN(L2SRAM)+LENGTH(L2SRAM));

REGION_ALIAS("REGION_TEXT",    PSRAM);
REGION_ALIAS("REGION_RODATA",  PSRAM);
REGION_ALIAS("REGION_DATA",    PSRAM);
REGION_ALIAS("REGION_BSS",     PSRAM);
REGION_ALIAS("REGION_PSRAM",   PSRAM);
REGION_ALIAS("REGION_L2SRAM",  L2SRAM);

ENTRY(Reset_Handler)
SECTIONS
{
 .text : {
  . = ALIGN(0x8) ;
  __stext = . ;
  KEEP(*startup.o(*.text*))
  . = ALIGN(0x8) ;
  __startupflash = .;

  __ramcopy_start = .;
  KEEP(*startup.o(*.vectors*))
  KEEP(*vectors.o(*.vectors*))
  *(EXCLUDE_FILE (
      *IQRDRLSDREmbedded.o
      *model_kws_8.o
      *model_vad_8.o*
      *rfft640_f32.o
      *tFeOpt_PFBA_Win320_1920_Delay960.o
      ) *.text)
   *(EXCLUDE_FILE (
      *IQRDRLSDREmbedded.o
      *model_kws_8.o
      *model_vad_8.o*
      *rfft640_f32.o
      *tFeOpt_PFBA_Win320_1920_Delay960.o
      ) *.text*)
   *(EXCLUDE_FILE (
      *IQRDRLSDREmbedded.o
      *model_kws_8.o
      *model_vad_8.o*
      *rfft640_f32.o
      *tFeOpt_PFBA_Win320_1920_Delay960.o
      ) *.text.*)
  /* *(.text)
  *(.text*)
  *(.text.*) */
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
  *(.eh_frame_hdr)
  KEEP (*(.eh_frame))
  *(.gcc_except_table .gcc_except_table.*)
  . = ALIGN(0x10) ;
  __srodata = .;
  *(.rdata)
  *(.rdata*)
  *(.rdata1)
  *(.rdata.*)
  *(.rodata)
  *(.rodata1)
  *(EXCLUDE_FILE (
      *IQRDRLSDREmbedded.o
      *model_kws_8.o
      *model_vad_8.o*
      *rfft640_f32.o
      *tFeOpt_PFBA_Win320_1920_Delay960.o
      ) *.rodata)
   *(EXCLUDE_FILE (
      *IQRDRLSDREmbedded.o
      *model_kws_8.o
      *model_vad_8.o*
      *rfft640_f32.o
      *tFeOpt_PFBA_Win320_1920_Delay960.o
      ) *.rodata.*)
   *(EXCLUDE_FILE (
      *IQRDRLSDREmbedded.o
      *model_kws_8.o
      *model_vad_8.o*
      *rfft640_f32.o
      *tFeOpt_PFBA_Win320_1920_Delay960.o
      ) *.rodata.str1.4)
   *(EXCLUDE_FILE (
      *IQRDRLSDREmbedded.o
      *model_kws_8.o
      *model_vad_8.o*
      *rfft640_f32.o
      *tFeOpt_PFBA_Win320_1920_Delay960.o
      ) *.srodata.*)
  /* *(.rodata*)
  *(.rodata.*)
  *(.rodata.str1.4)
  *(.srodata*) */
  *(.reallocate_vma*)
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

  . = ALIGN(0x8) ;
  __erodata = .;
  __rodata_end__ = .;
  . = ALIGN(0x10) ;
  __sdata = . ;
  __data_start__ = . ;
  data_start = . ;
  /* KEEP(*startup.o(*.vectors*)) */
  *(.got.plt)
  *(.got)
  *(.gnu.linkonce.r*)

  *(EXCLUDE_FILE (
      *IQRDRLSDREmbedded.o
      *model_kws_8.o
      *model_vad_8.o*
      *rfft640_f32.o
      *tFeOpt_PFBA_Win320_1920_Delay960.o
      ) *.data)
   *(EXCLUDE_FILE (
      *IQRDRLSDREmbedded.o
      *model_kws_8.o
      *model_vad_8.o*
      *rfft640_f32.o
      *tFeOpt_PFBA_Win320_1920_Delay960.o
      ) *.data*)
   *(EXCLUDE_FILE (
      *IQRDRLSDREmbedded.o
      *model_kws_8.o
      *model_vad_8.o*
      *rfft640_f32.o
      *tFeOpt_PFBA_Win320_1920_Delay960.o
      ) *.data.*)

  /* *(.data)
  *(.data*)
  *(.data.*) */
  *(.data1)
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
  . = ALIGN(0x8) ;
  KEEP (*(.eh_frame))
  *(.gcc_except_table .gcc_except_table.*)
  __edata = .;
  __data_end__ = .;
  __ramcopy_end = .;
 } > REGION_PSRAM AT > REGION_PSRAM

  .l2sram_text : {
     . = ALIGN(0x8) ;
     __ramcopy_l2sram_start = . ;
     *(.region_l2sram_text)


     *(EXCLUDE_FILE (
      *IQRDRLSDREmbedded.o
      *model_kws_8.o
      *model_vad_8.o*
      *rfft640_f32.o
      *tFeOpt_PFBA_Win320_1920_Delay960.o
      ) *.text*)

     *libmit_rtos.a:*.o(.text*)
     *libmit_rtos.a:*.o(.rodata*)
     . = ALIGN(0x10) ;
     *(.region_l2sram_data)
    *libmit_rtos.a:*.o(.data)
     . = ALIGN(0x10) ;
     __ramcopy_l2sram_end = . ;
  } > REGION_L2SRAM AT > REGION_PSRAM

 .bss : {
  . = ALIGN(0x8) ;
  __sbss = ALIGN(0x8) ;
  __bss_start__ = . ;
  *(.dynsbss)
  *(.sbss)
  *(.sbss.*)
  *(.scommon)
  *(.dynbss)

  *(EXCLUDE_FILE (
      *IQRDRLSDREmbedded.o
      *model_kws_8.o
      *model_vad_8.o*
      *rfft640_f32.o
      *tFeOpt_PFBA_Win320_1920_Delay960.o
      ) *.bss)
   *(EXCLUDE_FILE (
      *IQRDRLSDREmbedded.o
      *model_kws_8.o
      *model_vad_8.o*
      *rfft640_f32.o
      *tFeOpt_PFBA_Win320_1920_Delay960.o
      ) *.bss.*)
  /* *(.bss)
  *(.bss.*) */
  *(COMMON)
  . = ALIGN(0x8) ;
  __ebss = . ;
  __bss_end__ = .;
  __end = . ;
  end = . ;
 } > REGION_BSS

 ._user_heap_stack (NOLOAD) : {
  . = ALIGN(0x8) ;
   *(.stack*)
  . = ALIGN(0x8) ;
  __heap_start = .;
  . += __min_heap_size;
  . = ALIGN(0x8) ;
 } > REGION_BSS

  .l2sram_bss : {
     __l2sram_bss_start = . ;
     *(.region_l2sram_bss)
    *libmit_rtos.a:*.o(.bss*)
    *libmit_rtos.a:*.o(COMMON)
     . = ALIGN(0x10) ;
     __l2sram_bss_end = . ;

     __heap2_start = . ;
  } > REGION_L2SRAM
}

