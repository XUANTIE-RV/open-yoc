/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     wj_usi_com.c
 * @brief    header file for usi ll driver
 * @version  V1.0
 * @date     1. Mon 2020
 ******************************************************************************/

#include <drv/usi_usart.h>
#include <drv/irq.h>
#include <wj_usi_ll.h>
#include "wj_usi_com.h"

void wj_usi_irq_handler(void *arg)
{
    CSI_PARAM_CHK_NORETVAL(arg);
    csi_uart_t *usi = (csi_uart_t *)arg;
    ck_usi_regs_t *addr;
    addr = (ck_usi_regs_t *)HANDLE_REG_BASE(usi);

    switch (wj_usi_get_mode_sel(addr)) {
        case WJ_USI_MODE_SEL_UART:
            ck_usi_uart_irqhandler((csi_uart_t *)arg);
            break;

        case WJ_USI_MODE_SEL_I2C:
            ck_usi_i2c_irqhandler((csi_iic_t *)arg);
            break;

        case WJ_USI_MODE_SEL_SPI:
            ck_usi_spi_irqhandler((csi_spi_t *)arg);
            break;

        default:
            break;
    }
}

uint32_t usi_find_max_prime_num(uint32_t num)
{
    uint32_t i = 0U;

    for (i = 8U; i > 0U; i --) {
        if (!num) {
            i = 1U;
            break;
        }

        if (!(num % i)) {
            break;
        }

    }

    return i;
}

#ifdef CONFIG_PM
#define  USI_COMMON_REGS_NUM1   7U
#define  USI_UART_REGS_NUM      2U
#define  USI_IIC_REGS_NUM       8U
#define  USI_SPI_REGS_NUM       3U
#define  USI_COMMON_REGS_NUM    9U
csi_error_t usi_pm_action(csi_dev_t *dev, csi_pm_dev_action_t action)
{
    CSI_PARAM_CHK(dev, CSI_ERROR);

    ck_usi_regs_t *addr;
    addr = (ck_usi_regs_t *)dev->reg_base;
    csi_error_t ret = CSI_OK;
    csi_pm_dev_t *pm_dev = &dev->pm_dev;

    switch (action) {
        case PM_DEV_SUSPEND:
            csi_pm_dev_save_regs(pm_dev->reten_mem, (uint32_t *)dev->reg_base, USI_COMMON_REGS_NUM1);

            if (wj_usi_get_mode_sel(addr) == WJ_USI_MODE_SEL_UART) {
                csi_pm_dev_save_regs(pm_dev->reten_mem + USI_COMMON_REGS_NUM1, (uint32_t *)(dev->reg_base - USI_COMMON_REGS_NUM1 * 4), USI_UART_REGS_NUM);
            }

            if (wj_usi_get_mode_sel(addr) == WJ_USI_MODE_SEL_I2C) {
                csi_pm_dev_save_regs(pm_dev->reten_mem + USI_COMMON_REGS_NUM1 + USI_UART_REGS_NUM, (uint32_t *)(dev->reg_base - USI_UART_REGS_NUM * 4), USI_IIC_REGS_NUM);
            }

            if (wj_usi_get_mode_sel(addr) == WJ_USI_MODE_SEL_SPI) {
                csi_pm_dev_save_regs(pm_dev->reten_mem + USI_COMMON_REGS_NUM1 + USI_UART_REGS_NUM + USI_IIC_REGS_NUM, (uint32_t *)(dev->reg_base - USI_IIC_REGS_NUM * 4), USI_SPI_REGS_NUM);
            }

            csi_pm_dev_save_regs(pm_dev->reten_mem + USI_COMMON_REGS_NUM1 + USI_UART_REGS_NUM + USI_IIC_REGS_NUM + USI_SPI_REGS_NUM, (uint32_t *)(dev->reg_base - USI_IIC_REGS_NUM * 4), USI_COMMON_REGS_NUM);
            break;

        case PM_DEV_RESUME:
            csi_pm_dev_restore_regs(pm_dev->reten_mem, (uint32_t *)dev->reg_base, USI_COMMON_REGS_NUM1);

            if (wj_usi_get_mode_sel(addr) == WJ_USI_MODE_SEL_UART) {
                csi_pm_dev_restore_regs(pm_dev->reten_mem + USI_COMMON_REGS_NUM1, (uint32_t *)(dev->reg_base - USI_COMMON_REGS_NUM1 * 4), USI_UART_REGS_NUM);
            }

            if (wj_usi_get_mode_sel(addr) == WJ_USI_MODE_SEL_I2C) {
                csi_pm_dev_restore_regs(pm_dev->reten_mem + USI_COMMON_REGS_NUM1 + USI_UART_REGS_NUM, (uint32_t *)(dev->reg_base - USI_UART_REGS_NUM * 4), USI_IIC_REGS_NUM);
            }

            if (wj_usi_get_mode_sel(addr) == WJ_USI_MODE_SEL_SPI) {
                csi_pm_dev_restore_regs(pm_dev->reten_mem + USI_COMMON_REGS_NUM1 + USI_UART_REGS_NUM + USI_IIC_REGS_NUM, (uint32_t *)(dev->reg_base - USI_IIC_REGS_NUM * 4), USI_SPI_REGS_NUM);
            }

            csi_pm_dev_restore_regs(pm_dev->reten_mem + USI_COMMON_REGS_NUM1 + USI_UART_REGS_NUM + USI_IIC_REGS_NUM + USI_SPI_REGS_NUM, (uint32_t *)(dev->reg_base - USI_IIC_REGS_NUM * 4), USI_COMMON_REGS_NUM);

            break;

        default:
            ret = CSI_ERROR;
            break;
    }

    return ret;
}

csi_error_t usi_enable_pm(csi_dev_t *dev)
{
    return csi_pm_dev_register(dev, usi_pm_action, 100U, 0U);
}

void usi_disable_pm(csi_dev_t *dev)
{
    csi_pm_dev_unregister(dev);
}
#endif