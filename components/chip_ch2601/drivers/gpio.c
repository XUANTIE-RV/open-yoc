/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     dw_gpio.c
 * @brief
 * @version
 * @date     2020-01-08
 ******************************************************************************/

#include <string.h>

#include <csi_config.h>
#include <drv/gpio.h>
#include <drv/irq.h>
#include <drv/pin.h>

#include "dw_gpio_ll.h"

extern csi_error_t csi_pin_mode(pin_name_t pin_name, csi_gpio_mode_t mode);

static void dw_gpio_irqhandler(void *args)
{
    uint32_t bitmask;
    csi_gpio_t *handle = (csi_gpio_t *)args;
    dw_gpio_regs_t *reg = (dw_gpio_regs_t *)HANDLE_REG_BASE(handle);

    bitmask = dw_gpio_read_port_int_status(reg);

    /* clear all interrput */
    dw_gpio_clr_port_irq(reg, bitmask);

    /* execute the callback function */
    if (handle->callback) {
        handle->callback(handle, bitmask, handle->arg);
    }
}

csi_error_t csi_gpio_init(csi_gpio_t *gpio, uint32_t port_idx)
{
    CSI_PARAM_CHK(gpio, CSI_ERROR);

    csi_error_t ret = CSI_OK;

    if (target_get(DEV_DW_GPIO_TAG, port_idx, &gpio->dev) != CSI_OK) {
        ret = CSI_ERROR;
    }

    return ret;
}

void csi_gpio_uninit(csi_gpio_t *gpio)
{
    CSI_PARAM_CHK_NORETVAL(gpio);

    dw_gpio_regs_t *reg = (dw_gpio_regs_t *)HANDLE_REG_BASE(gpio);

    /* reset all related register*/
    dw_gpio_set_port_irq(reg, 0U);
    dw_gpio_set_port_msk(reg, 0U);
    dw_gpio_set_port_irq_sensitive(reg, 0U);
    dw_gpio_set_port_irq_polarity(reg, 0U);
    dw_gpio_write_output_port(reg, 0U);
    dw_gpio_set_port_direction(reg, 0U);
    dw_gpio_set_sync_level(reg, (bool)0);

    /* unregister irq */
    csi_irq_disable((uint32_t)gpio->dev.irq_num);
    csi_irq_detach((uint32_t)gpio->dev.irq_num);

    /* release handle */
    memset(gpio, 0, sizeof(csi_gpio_t));
}

csi_error_t csi_gpio_dir(csi_gpio_t *gpio, uint32_t pin_mask, csi_gpio_dir_t dir)
{
    CSI_PARAM_CHK(gpio, CSI_ERROR);
    CSI_PARAM_CHK(pin_mask, CSI_ERROR);

    csi_error_t ret = CSI_OK;

    dw_gpio_regs_t *reg = (dw_gpio_regs_t *)HANDLE_REG_BASE(gpio);
    uint32_t tmp = dw_gpio_read_port_direction(reg);

    switch (dir) {
        case GPIO_DIRECTION_INPUT:
            dw_gpio_set_port_direction(reg, tmp & (~pin_mask));
            break;

        case GPIO_DIRECTION_OUTPUT:
            dw_gpio_set_port_direction(reg, tmp | pin_mask);
            break;

        default:
            ret = CSI_UNSUPPORTED;
            break;
    }

    return ret;
}

csi_error_t csi_gpio_mode(csi_gpio_t *gpio, uint32_t pin_mask, csi_gpio_mode_t mode)
{
    CSI_PARAM_CHK(gpio, CSI_ERROR);
    CSI_PARAM_CHK(pin_mask, CSI_ERROR);

    csi_error_t ret = CSI_OK;
    csi_error_t temp;
    uint8_t offset = 0U;
    pin_name_t pin_name;

    /* set pin mode */
    while (pin_mask) {
        if (pin_mask & 0x01U) {
            pin_name = csi_pin_get_pinname_by_gpio(gpio->dev.idx, offset);

            if ((uint8_t)pin_name != 0xFFU) {
                temp = csi_pin_mode(pin_name, mode);

                if (temp == CSI_ERROR) { /* return CSI_ERROR if csi_pin_mode return CSI_ERROR */
                    ret = CSI_ERROR;
                    break;
                } else if (temp == CSI_UNSUPPORTED) { /* return CSI_UNSUPPORTED if csi_pin_mode return CSI_UNSUPPORTED */
                    ret = CSI_UNSUPPORTED;
                }
            }
        }

        pin_mask >>= 1U;
        offset++;
    }

    return ret;
}

csi_error_t csi_gpio_irq_mode(csi_gpio_t *gpio, uint32_t pin_mask, csi_gpio_irq_mode_t mode)
{
    CSI_PARAM_CHK(gpio, CSI_ERROR);

    uint32_t senstive, polarity;
    csi_error_t    ret = CSI_OK;
    dw_gpio_regs_t *reg = (dw_gpio_regs_t *)HANDLE_REG_BASE(gpio);

    senstive = dw_gpio_read_port_irq_sensitive(reg);
    polarity = dw_gpio_read_port_irq_polarity(reg);

    switch (mode) {
        /* rising edge interrupt mode */
        case GPIO_IRQ_MODE_RISING_EDGE:
            dw_gpio_set_port_irq_sensitive(reg, senstive | pin_mask);
            dw_gpio_set_port_irq_polarity(reg, polarity | pin_mask);
            break;

        /* falling edge interrupt mode */
        case GPIO_IRQ_MODE_FALLING_EDGE:
            dw_gpio_set_port_irq_sensitive(reg, senstive | pin_mask);
            dw_gpio_set_port_irq_polarity(reg, polarity & (~pin_mask));
            break;

        /* low level interrupt mode */
        case GPIO_IRQ_MODE_LOW_LEVEL:
            dw_gpio_set_port_irq_sensitive(reg, senstive & (~pin_mask));
            dw_gpio_set_port_irq_polarity(reg, polarity & (~pin_mask));
            break;

        /* high level interrupt mode */
        case GPIO_IRQ_MODE_HIGH_LEVEL:
            dw_gpio_set_port_irq_sensitive(reg, senstive & (~pin_mask));
            dw_gpio_set_port_irq_polarity(reg, polarity | pin_mask);
            break;

        /* double edge interrupt mode */
        case GPIO_IRQ_MODE_BOTH_EDGE:
        default:
            ret = CSI_UNSUPPORTED;
            break;
    }

    return ret;
}

csi_error_t csi_gpio_irq_enable(csi_gpio_t *gpio, uint32_t pin_mask, bool enable)
{
    CSI_PARAM_CHK(gpio, CSI_ERROR);
    CSI_PARAM_CHK(pin_mask, CSI_ERROR);

    dw_gpio_regs_t *reg = (dw_gpio_regs_t *)HANDLE_REG_BASE(gpio);
    uint32_t temp = dw_gpio_read_port_irq(reg);

    if (enable) {
        dw_gpio_set_port_irq(reg, temp | pin_mask);
    } else {
        dw_gpio_set_port_irq(reg, temp & (~pin_mask));
    }

    return CSI_OK;
}

csi_error_t csi_gpio_debounce(csi_gpio_t *gpio, uint32_t pin_mask, bool enable)
{
    CSI_PARAM_CHK(gpio, CSI_ERROR);
    CSI_PARAM_CHK(pin_mask, CSI_ERROR);

    return CSI_UNSUPPORTED;
}

void csi_gpio_toggle(csi_gpio_t *gpio, uint32_t pin_mask)
{
    CSI_PARAM_CHK_NORETVAL(gpio);
    CSI_PARAM_CHK_NORETVAL(pin_mask);

    dw_gpio_regs_t *reg = (dw_gpio_regs_t *)HANDLE_REG_BASE(gpio);
    uint32_t tmp = dw_gpio_read_output_port(reg);

    dw_gpio_write_output_port(reg, tmp ^ pin_mask);
}

void  csi_gpio_write(csi_gpio_t *gpio, uint32_t pin_mask, csi_gpio_pin_state_t value)
{
    CSI_PARAM_CHK_NORETVAL(gpio);
    CSI_PARAM_CHK_NORETVAL(pin_mask);

    dw_gpio_regs_t *reg = (dw_gpio_regs_t *)HANDLE_REG_BASE(gpio);
    uint32_t tmp = dw_gpio_read_output_port(reg);

    if (value == 1) {
        dw_gpio_write_output_port(reg, tmp | pin_mask);
    } else {
        dw_gpio_write_output_port(reg, tmp & (~pin_mask));
    }
}

uint32_t csi_gpio_read(csi_gpio_t *gpio, uint32_t pin_mask)
{
    CSI_PARAM_CHK(gpio, CSI_ERROR);
    CSI_PARAM_CHK(pin_mask, CSI_ERROR);

    dw_gpio_regs_t *reg = (dw_gpio_regs_t *)HANDLE_REG_BASE(gpio);
    return dw_gpio_read_input_port(reg) & pin_mask;
}

csi_error_t  csi_gpio_attach_callback(csi_gpio_t *gpio, void *callback, void *arg)
{
    CSI_PARAM_CHK(gpio, CSI_ERROR);
    CSI_PARAM_CHK(callback, CSI_ERROR);

    dw_gpio_regs_t *reg = (dw_gpio_regs_t *)HANDLE_REG_BASE(gpio);

    /* clear interrput status before enable irq */
    dw_gpio_clr_port_irq(reg, 0U);

    gpio->callback = callback;
    gpio->arg      = arg;

    csi_irq_attach((uint32_t)gpio->dev.irq_num, &dw_gpio_irqhandler, &gpio->dev);
    csi_irq_enable((uint32_t)gpio->dev.irq_num);

    return CSI_OK;
}

void csi_gpio_detach_callback(csi_gpio_t *gpio)
{
    CSI_PARAM_CHK_NORETVAL(gpio);

    gpio->callback = NULL;
    gpio->arg      = NULL;
}

#ifdef CONFIG_PM
csi_error_t dw_gpio_pm_action(csi_dev_t *dev, csi_pm_dev_action_t action)
{
    CSI_PARAM_CHK(dev, CSI_ERROR);

    csi_error_t ret = CSI_OK;
    csi_pm_dev_t *pm_dev = &dev->pm_dev;

    switch (action) {
        case PM_DEV_SUSPEND:
            csi_pm_dev_save_regs(pm_dev->reten_mem, (uint32_t *)dev->reg_base, 3U);
            csi_pm_dev_save_regs(pm_dev->reten_mem + 3, (uint32_t *)(dev->reg_base + 0x30U), 4U);
            break;

        case PM_DEV_RESUME:
            csi_pm_dev_restore_regs(pm_dev->reten_mem, (uint32_t *)dev->reg_base, 3U);
            csi_pm_dev_restore_regs(pm_dev->reten_mem + 3, (uint32_t *)(dev->reg_base + 0x30U), 4U);
            break;

        default:
            ret = CSI_ERROR;
            break;
    }

    return ret;
}

csi_error_t csi_gpio_enable_pm(csi_gpio_t *gpio)
{
    return csi_pm_dev_register(&gpio->dev, dw_gpio_pm_action, 28U, 0U);
}

void csi_gpio_disable_pm(csi_gpio_t *gpio)
{
    csi_pm_dev_unregister(&gpio->dev);
}
#endif
