/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
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
#include <csi_config.h>

void drv_reboot(void)
{
    uint32_t irq_flag = 0;

    irq_flag = csi_irq_save();

#ifdef CONFIG_CPU_CK804EF
    wdt_handle_t wdt_handle;
    wdt_handle = csi_wdt_initialize(0, NULL);
    csi_wdt_start(wdt_handle);
    csi_wdt_set_timeout(wdt_handle, 0);
#endif

    /* waiting for reboot */
    while (1);

    csi_irq_restore(irq_flag);
}
