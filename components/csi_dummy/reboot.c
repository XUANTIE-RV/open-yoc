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
#include <csi_config.h>

void drv_reboot(void)
{
    uint32_t irq_flag = 0;

    irq_flag = csi_irq_save();


    /* waiting for reboot */
    while (1);

    csi_irq_restore(irq_flag);
}
