/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     gpio_pin.c
 * @brief
 * @version
 * @date     2020-06-28
 ******************************************************************************/
#include <string.h>

#include <csi_config.h>
#include <drv/gpio_pin.h>
#include <drv/gpio.h>
#include <drv/irq.h>
#include <drv/pin.h>

#include "dw_gpio_ll.h"

typedef struct {
    csi_gpio_t     gpio;
    csi_gpio_pin_t *pin[32];
} csi_gpio_manage_t;

csi_gpio_manage_t g_gpio_manage[CONFIG_GPIO_NUM];

extern csi_error_t csi_pin_mode(pin_name_t pin_name, csi_gpio_mode_t mode);

static void gpio_manage_callback(csi_gpio_t *gpio, uint32_t pins, void *arg)
{
    csi_gpio_pin_t **pin;
    uint8_t pin_idx = 0U;

    do {
        if (pins == 0U) {
            break ;
        }

        pin = &g_gpio_manage[gpio->dev.idx].pin[0];

        do {
            while (!(pins & 0x1U)) {
                pins >>= 1U;
                pin_idx++;
            }

            if (pin[pin_idx] && pin[pin_idx]->callback) {
                pin[pin_idx]->callback(pin[pin_idx], pin[pin_idx]->arg);
            }

            pins >>= 1U;
            pin_idx++;
        } while (pins);
    } while(0);
}

csi_error_t csi_gpio_pin_init(csi_gpio_pin_t *pin, pin_name_t pin_name)
{
    CSI_PARAM_CHK(pin, CSI_ERROR);

    csi_error_t ret = CSI_OK;
    size_t   state;
    uint32_t idx;

    idx = csi_pin_get_gpio_channel(pin_name);

    if (idx == 0xFFFFFFFFU) {
        ret = CSI_ERROR;
    } else {
        pin->pin_idx = idx;
    }

    idx = csi_pin_get_gpio_devidx(pin_name);

    if (idx == 0xFFFFFFFFU) {
        ret = CSI_ERROR;
    } else {
        state = csi_irq_save();

        if (!g_gpio_manage[idx].gpio.dev.reg_base) {
            csi_gpio_init(&g_gpio_manage[idx].gpio, idx);
            csi_gpio_attach_callback(&g_gpio_manage[idx].gpio, gpio_manage_callback, NULL);
        }

        csi_irq_restore(state);
        pin->gpio = &g_gpio_manage[idx].gpio;
        g_gpio_manage[idx].pin[pin->pin_idx] = pin;
    }

    return ret;
}

void csi_gpio_pin_uninit(csi_gpio_pin_t *pin)
{
    CSI_PARAM_CHK_NORETVAL(pin);

    size_t   state;

    for (uint8_t i = 0U; i < CONFIG_GPIO_NUM; i++) {
        state = csi_irq_save();

        if (&g_gpio_manage[i].gpio == pin->gpio) {
            g_gpio_manage[i].pin[pin->pin_idx] = NULL;
        }

        csi_irq_restore(state);
    }

    pin->pin_idx  = 0U;
    pin->callback = NULL;
    pin->arg = NULL;
}

csi_error_t csi_gpio_pin_attach_callback(csi_gpio_pin_t *pin, void *callback, void *arg)
{
    CSI_PARAM_CHK(pin, CSI_ERROR);
    CSI_PARAM_CHK(callback, CSI_ERROR);

    uint32_t bitmask;

    dw_gpio_regs_t *reg = (dw_gpio_regs_t *)HANDLE_REG_BASE(pin->gpio);

    bitmask = dw_gpio_read_port_int_status(reg);

    /* clear all interrput */
    dw_gpio_clr_port_irq(reg, bitmask);

    pin->callback = callback;
    pin->arg      = arg;

    return CSI_OK;
}

csi_error_t csi_gpio_pin_dir(csi_gpio_pin_t *pin, csi_gpio_dir_t dir)
{
    CSI_PARAM_CHK(pin, CSI_ERROR);

    return csi_gpio_dir(pin->gpio, (uint32_t)1U << pin->pin_idx, dir);
}

csi_error_t csi_gpio_pin_mode(csi_gpio_pin_t *pin, csi_gpio_mode_t mode)
{
    CSI_PARAM_CHK(pin, CSI_ERROR);

    return csi_gpio_mode(pin->gpio, (uint32_t)1U << pin->pin_idx, mode);
}

csi_error_t csi_gpio_pin_irq_mode(csi_gpio_pin_t *pin, csi_gpio_irq_mode_t mode)
{
    CSI_PARAM_CHK(pin, CSI_ERROR);

    return csi_gpio_irq_mode(pin->gpio, (uint32_t)1U << pin->pin_idx, mode);
}

csi_error_t csi_gpio_pin_irq_enable(csi_gpio_pin_t *pin,  bool enable)
{
    CSI_PARAM_CHK(pin, CSI_ERROR);

    return csi_gpio_irq_enable(pin->gpio, (uint32_t)1U << pin->pin_idx, enable);
}

csi_error_t csi_gpio_pin_debounce(csi_gpio_pin_t *pin, bool enable)
{
    CSI_PARAM_CHK(pin, CSI_ERROR);

    return csi_gpio_debounce(pin->gpio, (uint32_t)1U << pin->pin_idx, enable);
}

void  csi_gpio_pin_write(csi_gpio_pin_t *pin, csi_gpio_pin_state_t value)
{
    CSI_PARAM_CHK_NORETVAL(pin);

    return csi_gpio_write(pin->gpio, (uint32_t)1U << pin->pin_idx, value);
}

void csi_gpio_pin_toggle(csi_gpio_pin_t *pin)
{
    CSI_PARAM_CHK_NORETVAL(pin);

    return csi_gpio_toggle(pin->gpio, (uint32_t)1U << pin->pin_idx);
}

csi_gpio_pin_state_t csi_gpio_pin_read(csi_gpio_pin_t *pin)
{
    csi_gpio_pin_state_t state;

    if (csi_gpio_read(pin->gpio, (uint32_t)1U << pin->pin_idx) != 0U) {
        state = GPIO_PIN_HIGH;
    } else {
        state = GPIO_PIN_LOW;
    }

    return state;
}

