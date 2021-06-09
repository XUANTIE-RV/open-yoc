/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */


/******************************************************************************
 * @file     board_init.c
 * @brief    CSI Source File for board init
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <soc.h>
#include <board_config.h>
#include <pinmux.h>
#include <devices/devicelist.h>
#include <aos/aos.h>

#ifdef CONFIG_GENIE_MESH_ENABLE
#include <aos/hal/flash.h>

const hal_logic_partition_t hal_partitions[] =
{

         [HAL_PARTITION_BT_FIRMWARE] =
            {
                .partition_owner = HAL_FLASH_SPI,
                .partition_description = "otp",
                .partition_start_addr = 0x11004000,
                .partition_length = 0x1000, //4k bytes
                .partition_options = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
            },
#if 0
        [HAL_PARTITION_PARAMETER_2] =
            {
                .partition_owner = HAL_FLASH_SPI,
                .partition_description = "kv",
                .partition_start_addr = 0x1107D000,
                .partition_length = 0x3000, //12k bytes
                .partition_options = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
            },
#endif
        [HAL_PARTITION_OTA_TEMP] =
            {
                .partition_owner = HAL_FLASH_SPI,
                .partition_description = "ota",
                .partition_start_addr = 0x11046000,
                .partition_length = 0x37000,
                .partition_options = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
}};
#endif

#ifdef CONFIG_PM_SLEEP
extern int sys_soc_init();
extern int sys_soc_resume(int pm_state);
extern int sys_soc_suspend(uint32_t suspend_tick);
__attribute__((section(".__sram.code"))) static void lpm_handle(void)
{
    int pm_state;
    int ticks;

    aos_kernel_sched_suspend();

    uint32_t suspend_tick = aos_kernel_suspend();

    pm_state = sys_soc_suspend(suspend_tick);
    ticks = sys_soc_resume(pm_state);

    if (ticks) {
        aos_kernel_resume(ticks);
    }

    aos_kernel_sched_resume();

    return;
}

__attribute__((section(".__sram.code"))) void krhino_idle_hook(void)
{
#ifdef CONFIG_BT_ECC
    extern void ecc_work_handler();
    ecc_work_handler();
#endif
    lpm_handle();
}

#else
void krhino_idle_hook(void)
{
#ifdef CONFIG_BT_ECC
    extern void ecc_work_handler();
    ecc_work_handler();
#endif
}
#endif

void board_init(void)
{
    drv_pinmux_config(CONSOLE_TXD, CONSOLE_TXD_FUNC);
    drv_pinmux_config(CONSOLE_RXD, CONSOLE_RXD_FUNC);
#ifdef CONFIG_PM_SLEEP
    sys_soc_init();
#endif

    extern int hci_driver_phy6220_register(int idx);
    extern int hci_h4_driver_init();
    hci_driver_phy6220_register(0);
    hci_h4_driver_init();

    /* USE CONFIG_WDT=1 to enable wdt */
    extern void wdt_init(void);
    wdt_init();
}

