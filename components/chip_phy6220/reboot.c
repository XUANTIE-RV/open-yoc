/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     reboot.c
 * @brief    source file for the reboot
 * @version  V1.0
 * @date     04. April 2019
 * @vendor   csky
 * @chip     pangu
 ******************************************************************************/

#include <soc.h>
#include <drv/wdt.h>
#include <mcu_phy_bumbee.h>
#define RST_FROM_ROM_BOOT  0
#define RST_FROM_APP_RST_HANDLER 1

extern size_t cpu_intrpt_save(void);
extern void set_sleep_flag(int flag);

__attribute__((section(".__sram.code.drv_reboot"))) void drv_reboot(int cmd)
{
    csi_irq_save();

    if(cmd==RST_FROM_ROM_BOOT)
    {
        set_sleep_flag(0);
    }
    else
    {
        set_sleep_flag(1);
    }

    volatile int dly = 100;

    //cache rst
    AP_PCR->CACHE_RST = 0;

    while (dly--) {};

    AP_PCR->CACHE_RST = 0x03;

    //cache flush tag
    AP_CACHE->CTRL0 = 0x03;

    //cache enable
    AP_PCR->CACHE_BYPASS = 1;

    *(volatile uint32_t *) 0x40000004 = 0x00;

    /* waiting for reboot */
    while (1);
}
