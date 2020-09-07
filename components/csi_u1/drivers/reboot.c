/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     reboot.c
 * @brief    source file for the reboot
 * @version  V1.0
 * @date     04. April 2019
 ******************************************************************************/

#include <soc.h>
#include <io.h>

void drv_reboot(int cmd)
{
    if (cmd == 1) {
        putreg32(CSKY_PWR_RESET_ROMBOOT, (uint32_t *)CSKY_PWR_RES_REG2);
        putreg32((CSKY_PWR_SFRST_RESET << 16) | CSKY_PWR_SFRST_RESET, \
                 (uint32_t *)CSKY_PWR_SFRST_CTL);
    } else {
        putreg32(CSKY_PWR_RESET_NORMAL, (uint32_t *)CSKY_PWR_RES_REG2);
        putreg32((CSKY_PWR_SFRST_RESET << 16) | CSKY_PWR_SFRST_RESET, \
                 (uint32_t *)CSKY_PWR_SFRST_CTL);
    }
}
