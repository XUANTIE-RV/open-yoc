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
    FLASH (rxai!w)  : ORIGIN = 0x58023000, LENGTH = 3M
    
    OCRAM(!rx)      : ORIGIN = 0x62020000, LENGTH = 64K
    WRAM(!rx)       : ORIGIN = 0x22030000, LENGTH = 96K-512
    
    VRAM(!rx)       : ORIGIN = 0x7F000000, LENGTH = 32K
    
    PSRAM(rx)       : ORIGIN = 0x54020000, LENGTH = 12M-128K
}

__min_heap_size = 0x200;
BOOT2_PT_ADDR = 0x22047E00;
__RFTLV_SIZE_OFFSET = (0x400);

PROVIDE (__ocram_end = ORIGIN(OCRAM)+LENGTH(OCRAM));
PROVIDE (__wram_end  = ORIGIN(WRAM)+LENGTH(WRAM)); 
PROVIDE (__l2ram_end = ORIGIN(L2RAM)+LENGTH(L2RAM)); 
PROVIDE (__vram_end  = ORIGIN(VRAM)+LENGTH(VRAM)); 
PROVIDE (__psram_end  = ORIGIN(PSRAM)+LENGTH(PSRAM)); 

PROVIDE (__heap_end  = __psram_end);

PROVIDE (__heap2_end = __wram_end);

PROVIDE (__heap3_start = ORIGIN(OCRAM));
PROVIDE (__heap3_end = __ocram_end);

PROVIDE (__heap4_start = ORIGIN(VRAM));
PROVIDE (__heap4_end = __vram_end);

PROVIDE (__heap5_start = __psram_end);
PROVIDE (__heap5_end = __psram_end);

PROVIDE( _ld_bl_static_cfg_entry_start = _bl_static_fw_cfg_entry_start );
PROVIDE( _ld_bl_static_cfg_entry_end   = _bl_static_fw_cfg_entry_end );

PROVIDE ( __boot2_pt_addr_src = BOOT2_PT_ADDR );

REGION_ALIAS("REGION_TEXT",    PSRAM);
REGION_ALIAS("REGION_RODATA",  PSRAM);
REGION_ALIAS("REGION_DATA",    PSRAM);
REGION_ALIAS("REGION_BSS",     PSRAM);
REGION_ALIAS("RAM_WIFI",       WRAM);

ENTRY(Reset_Handler)
SECTIONS
{

  .init : {
  . = ALIGN(0x4) ;
  __stext = . ;
  KEEP(*startup.o(*.text*))
  KEEP(*vectors.o(*.text*))
  } > REGION_TEXT 

  .rfparam : {
  . = ALIGN(0x4) ;
  . = ORIGIN(REGION_TEXT) + __RFTLV_SIZE_OFFSET;
  KEEP (*(.rfparam))    
  } > REGION_TEXT 

 .text : {
  . = ALIGN(0x4) ;
  KEEP(*port_s.o(*.text*))
  KEEP(*system.o(*.text*))
  KEEP(*trap_c.o(*.text*))
  KEEP(*k_stats.o(*.text*))
  KEEP(*libaos.a:*main.o(*.text*))
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
 .eh_frame_hdr : {
  *(.eh_frame_hdr)
 } > REGION_TEXT
 .eh_frame : ONLY_IF_RO {
  KEEP (*(.eh_frame))
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
  *(.static_cli_cmds*)
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

    _bt_gatt_service_static_list_start = .;
    KEEP(*(SORT_BY_NAME("._bt_gatt_service_static.static.*")))
    _bt_gatt_service_static_list_end = .;
    _bt_l2cap_fixed_chan_list_start = .;
    KEEP(*(SORT_BY_NAME("._bt_l2cap_fixed_chan.static.*")))
    _bt_l2cap_fixed_chan_list_end = .;
    _bt_l2cap_br_fixed_chan_list_start = .;
    KEEP(*(SORT_BY_NAME("._bt_l2cap_br_fixed_chan.static.*")))
    _bt_l2cap_br_fixed_chan_list_end = .;

    /* static fw attribute entry */
    . = ALIGN(4);
    _bl_static_fw_cfg_entry_start = .;
    KEEP(*(.wifi.cfg.entry))
    _bl_static_fw_cfg_entry_end = .;
    . = ALIGN(4);
        
  __erodata = .;
  __rodata_end__ = .;
 } > REGION_RODATA
 
  .wifibss  (NOLOAD) : {
    PROVIDE( __wifi_bss_start = ADDR(.wifibss) );
    PROVIDE( __wifi_bss_end = ADDR(.wifibss) + SIZEOF(.wifibss) );
    KEEP(*ipc_shared.o(COMMON))
    KEEP(*sdu_shared.o(COMMON))
    KEEP(*hal_desc.o(COMMON))
    KEEP(*txl_buffer_shared.o(COMMON))
    KEEP(*txl_frame_shared.o(COMMON))
    KEEP(*scan_shared.o(COMMON))
    KEEP(*scanu_shared.o(COMMON))
    KEEP(*mfp_bip.o(COMMON))
    KEEP(*me_mic.o(COMMON))
    KEEP(*bl_sta_mgmt_others.o(COMMON))
    KEEP(*bl_pmk_mgmt.o(COMMON))
    KEEP(*bl_pmk_mgmt_internal.o(COMMON))
    KEEP(*libwifi_drv.a:bl_utils.o(COMMON))
    KEEP(*libwifi_drv.a:bl_utils.o(.bss*))
    KEEP(*(.wifi_ram*))
    . = ALIGN(16);
  } > RAM_WIFI
  
   ._user_heap2 : {
  . = ALIGN(0x8) ;
  __heap2_start = .;
  . += __min_heap_size;
  . = ALIGN(0x8) ;
 } > RAM_WIFI
 
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
 } > REGION_DATA AT > REGION_RODATA
 .eh_frame : ONLY_IF_RW {
  KEEP (*(.eh_frame))
 } > REGION_DATA AT > REGION_RODATA
 .gcc_except_table : ONLY_IF_RW {
  *(.gcc_except_table .gcc_except_table.*)
  __edata = .;
  __data_end__ = .;
 } > REGION_DATA AT > REGION_RODATA
 
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
 ._user_heap1 : {
  . = ALIGN(0x8) ;
  __heap_start = .;
  . += __min_heap_size;
  . = ALIGN(0x8) ;
 } > REGION_BSS
}
