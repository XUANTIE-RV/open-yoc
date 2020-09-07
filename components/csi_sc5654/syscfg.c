/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
/******************************************************************************
 * @file     syscfg.c
 * @brief    source file for syscfg.
 * @version  V1.0
 * @date     22. June 2019
 ******************************************************************************/

#include <stdint.h>
#include <syscfg.h>
#include <soc.h>
#include <csi_core.h>

void drv_clk_enable(uint32_t module)
{
    silan_syscfg_reg_t *syscfg = (silan_syscfg_reg_t *)SILAN_SYSCFG_BASE;

    if (module <= 31) {
        syscfg->CTR_REG0 |= (0x1U << module);
    } else if (module == 32) {
        syscfg->CTR_REG1 |= 0x1U;
    }
}

void drv_clk_disable(uint32_t module)
{
    silan_syscfg_reg_t *syscfg = (silan_syscfg_reg_t *)SILAN_SYSCFG_BASE;

    if (module <= 31) {
        syscfg->CTR_REG0 &= ~(0x1U << module);
    } else if (module == 32) {
        syscfg->CTR_REG1 &= ~0x1U;
    }
}
