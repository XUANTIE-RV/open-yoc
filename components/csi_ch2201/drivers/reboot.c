/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     reboot.c
 * @brief    source file for the reboot
 * @version  V1.0
 * @date     04. April 2019
 ******************************************************************************/

#include <soc.h>
#include <drv/wdt.h>

void drv_reboot(void)
{
    wdt_handle_t wdt_handle;
    uint32_t irq_flag = 0;

    irq_flag = csi_irq_save();

    wdt_handle = csi_wdt_initialize(0, NULL);
    csi_wdt_start(wdt_handle);
    csi_wdt_set_timeout(wdt_handle, 0);

    /* waiting for reboot */
    while (1);

    csi_irq_restore(irq_flag);
}
