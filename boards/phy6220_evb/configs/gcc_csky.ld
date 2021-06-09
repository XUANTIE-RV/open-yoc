/*
 * Copyright (C) 2017 C-SKY Microsystems Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/******************************************************************************
 * @file     gcc_csky.ld
 * @brief    csky linker file
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/
MEMORY
{
	XROM1  : ORIGIN = 0x11011000 , LENGTH = 0x35000   /* XIPROM  212KB */
	SRAM   : ORIGIN = 0x1FFF1a40 , LENGTH = 0xe5c0      /* SRAM  57.4KB */
}

__min_heap_size = 0x1000;
PROVIDE (__ram_end  = 0x20000000);
PROVIDE (__heap_end = __ram_end);

REGION_ALIAS("REGION_TEXT",    XROM1);
REGION_ALIAS("REGION_RODATA",  XROM1);
REGION_ALIAS("REGION_DATA",    SRAM);
REGION_ALIAS("REGION_BSS",     SRAM);
REGION_ALIAS("REGION_XTEXT",    XROM1);
REGION_ALIAS("REGION_XRODATA",  XROM1);

ENTRY(Reset_Handler)
SECTIONS
{
 .rodata : {
  . = ALIGN(0x4) ;
  KEEP(*startup.o(.vectors))
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
  KEEP (*crtbegin.o(.ctors))
  KEEP (*crtbegin?.o(.ctors))
  KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .ctors))
  KEEP (*(SORT(.ctors.*)))
  KEEP (*(.ctors))
  KEEP (*crtbegin.o(.dtors))
  KEEP (*crtbegin?.o(.dtors))
  KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .dtors))
  KEEP (*(SORT(.dtors.*)))
  KEEP (*(.dtors))
  . = ALIGN(0x4) ;
  __erodata = .;
  __rodata_end__ = .;
 } > REGION_RODATA
 .data : {
  . = ALIGN(0x4) ;
  __sdata = . ;
  __data_start__ = . ;
  data_start = . ;
  data_text_start = . ;
  //*librhino.a:*systick.o(.text.*)
  //*librhino.a:*hook_impl.o(.text.*)
  //*librhino.a:*hook_weak.o(.text.*)
  *librhino.a:*k_err.o(.text.*)
  //*librhino.a:*k_idle.o(.text.*)
  //*librhino.a:*k_ringbuf.o(.text.*)
  *librhino.a:*k_pend.o(.text.*)
  *librhino.a:*k_sem.o(.text.krhino_sem_give .text.sem_give )
  *librhino.a:*k_sched.o(.text.*)

  *libaos.a:*hook_impl.o(.text.*)
  *libchip_phy6220.a:*system.o(.text.hal_cache_init )
  *csky_case_uqi.o(*)
  *csky_case_shi.o(*)

  . = ALIGN(0x800) ;
  *libchip_phy6220.a:*system.o(.bss.PHY6220_Vectors)

  *librhino.a:*k_tick.o(.text.*)
  //*librhino.a:*k_buf_queue.o(.text.*)
  //*librhino.a:*k_mutex.o(.text.*)
  *librhino_arch_*.a:*port_s.o(*)

  *librhino.a:*k_time.o(.text.krhino_tick_proc)
  *librhino.a:*k_sys.o(.text.krhino_intrpt_exit .text.krhino_intrpt_enter)
  *libchip_phy6220.a:*isr.o(.text.*)
  *libchip_phy6220.a:*port_s.o(*)

  //*libchip_phy6220.a:*os_port.o(*.text*)
  //*libchip_phy6220.a:*pinmux.o(*.text*)
  //*libchip_phy6220.a:*gpio.o(*.text*)
  //*libchip_phy6220.a:*dw_usart.o(.text.csi_usart_receive_query .text.csi_usart_send .text.csi_usart_putchar .text.dw_usart_intr_recv_data .text.dw_usart_irqhandler .text.usart_wait_timeout*)
  //*libchip_phy6220.a:*device.o(.text.device_lock .text.device_unlock .text.device_find .text.device_valid)
  *libchip_phy6220.a:*pm.o(.text.*)
  *libchip_phy6220.a:*phy_pmu.o(.text.*)
  *libchip_phy6220.a:*lpm_arch_reg_save.o(*)
  *libchip_phy6220.a:*clock.o(.text.hal_clk_gate_enable)
  *libchip_phy6220.a:*system.o(.text.hal_wakeup_irq_config .text.hal_mpu_config)

  //*libaos.a:*uservice.o(.text.*)
  //*libaos.a:*lpm.o(.text.lpm_handle)
  
  //*libaos.a:*rpc.o(.text.*)
  //*libaos.a:*ringbuffer.o(.text.ringbuffer_available_read_space .text.ringbuffer_write .text.ringbuffer_read)
  //*libaos.a:*uart.o(.text.uart_recv .text.uart_send)
  //*libaos.a:*kernel.o(.text.rhino2stderrno .text.aos_task_self .text.aos_mutex_lock .text.aos_mutex_unlock .text.aos_sem_new .text.aos_sem_free .text.aos_mutex_is_valid .text.aos_sem_wait .text.aos_sem_signal .text.aos_sem_is_valid .text.aos_event_get .text.aos_event_set .text.aos_queue_send .text.aos_queue_recv .text.aos_now_ms .text.aos_msleep .text.aos_kernel_sched_suspend .text.aos_kernel_sched_resume .text.aos_task_yield .text.aos_kernel_tick2ms .text.aos_kernel_ms2tick .text.aos_kernel_suspend .text.aos_kernel_resume)
  *libaos.a:*aos_rhino.o(.text.aos_kernel_sched_suspend .text.aos_kernel_sched_resume .text.aos_kernel_suspend .text.aos_kernel_resume)

  //*libdrivers.a:*uart_drv.o(.text.usart_csky_event_cb_fun .text.uart_csky_send .text.uart_csky_recv)

  *libdrv_bt_phy6220*.a:*ll_patch.o(.text.*)
  *libdrv_bt_phy6220*.a:*rf_phy_driver.o(.text.*)
  *libdrv_bt_phy6220*.a:*phy6220_hci.o(.text.osal_set_event1)

  *libble_host.a:*h4_driver.o(.text._hci_recv_event)
  //*libble_host.a:*aos_port.o(.text.*)
  //*libble_host.a:*atomic_c.o(.text.*)
  //*libble_host.a:*hci_core.o(.text.bt_hci_num_complete_packets .text.bt_recv .text.hci_event .text.handle_event .text.hci_le_meta_event .text.le_adv_recv)
  //*libble_host.a:*conn.o(.text.send_frag)
  //*libble_host.a:*poll.o(.text.*)
  //*libble_host.a:*work.o(.text.*)
  //*libble_host.a:*buf.o(.text.net_buf_alloc_len* .text.net_buf_slist_put .text.net_buf_slist_get)
  *(*__udiv32*)

  *libminilibc.a:*memcpy_fast.o(*)
  *(.__sram.code)
  data_text_end = . ;

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
  __data_copy_first_start__ = .;
  *(.__data_copy_data_first__);
  KEEP (*(.__data_copy_first__))
  __data_copy_first_end__ = .;
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
 .text : {
  . = ALIGN(0x4) ;
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
 .eh_frame_hdr : {
  *(.eh_frame_hdr)
 } > REGION_TEXT
 .eh_frame : ONLY_IF_RO {
  KEEP (*(.eh_frame))
 } > REGION_TEXT
 .gcc_except_table : ONLY_IF_RO {
  *(.gcc_except_table .gcc_except_table.*)
 } > REGION_TEXT
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
  . = ALIGN(0x4) ;
  __heap_start = .;
  . += __min_heap_size;
  . = ALIGN(0x4) ;
 } > REGION_BSS
}
