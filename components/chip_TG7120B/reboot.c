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
extern size_t cpu_intrpt_save(void);

__attribute__((section(".__sram.code"))) void drv_reboot(int cmd)
{
    csi_irq_save();


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
