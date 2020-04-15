/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     reboot.c
 * @brief    source file for the reboot
 * @version  V1.0
 * @date     04. April 2019
 ******************************************************************************/

#include <sysctrl.h>
#include <soc.h>
#include <csi_core.h>

void drv_reboot(void)
{
    uint32_t sys_ctr2;

    __disable_irq();

    silan_sysctl_reg_t *sysctl = (silan_sysctl_reg_t *)SILAN_SYSCTRL_BASE;

    sysctl->SYS_LOCK = SYS_LOCK_UNLOCK_MAGIC;
    sysctl->WDT_TG = 5000;

    sys_ctr2 = sysctl->SYS_CTR2;
    sys_ctr2 |= SYS_CTL2_WDT_RESET_EN_Msk;
    sysctl->SYS_CTR2 = sys_ctr2;

    sysctl->SYS_LOCK = SYS_LOCK_LOCK_MAGIC;

    while(1);
}
