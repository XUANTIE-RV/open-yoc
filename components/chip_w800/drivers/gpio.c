/*
 * Copyright (C) 2020 FishSemi Inc. All rights reserved.
 */
/******************************************************************************
 * @file     dw_gpio.c
 * @brief    CSI Source File for GPIO Driver
 * @version  V1.0
 * @date     7. Feb 2020
 ******************************************************************************/
#include "wm_type_def.h"
#include "wm_io.h"
#include "wm_gpio.h"

#ifdef bool
#undef bool
#endif

#include <csi_config.h>
#include <stdbool.h>
#include <stdio.h>
#include "drv/gpio.h"
#include "drv/irq.h"
#include "drv/pmu.h"
#include "csi_core.h"
#include "pin_name.h"
#include <aos/aos.h>
#include <soc.h>

#define ERR_GPIO(errno) (CSI_DRV_ERRNO_GPIO_BASE | errno)

typedef struct {
    uint32_t base;
    uint32_t irq;

    gpio_event_cb_t cb;
    uint8_t     pinidx;
} dw_gpio_pin_priv_t;


extern int32_t target_gpio_init(int32_t gpio_pin, uint32_t *base, uint32_t *irq, void **handler);

static dw_gpio_pin_priv_t gpio_pin_handle[CONFIG_GPIO_NUM];

static void gpio_irq_callback(void *arg)
{
    dw_gpio_pin_priv_t *gpio_pin_priv = (dw_gpio_pin_priv_t *)arg;
    u8 irq_status = tls_get_gpio_irq_status(gpio_pin_priv->pinidx);
    if (irq_status)
    {
        tls_clr_gpio_irq_status(gpio_pin_priv->pinidx);
        if (gpio_pin_priv->cb)
            gpio_pin_priv->cb(gpio_pin_priv->pinidx);
    }
}

/**
  \brief       Initialize GPIO handle.
  \param[in]   gpio_pin    Pointer to the int32_t.
  \param[in]   cb_event    Pointer to \ref gpio_event_cb_t.
  \param[in]   arg    Pointer to \ref arg used for the callback.
  \return      gpio_pin_handle
*/
gpio_pin_handle_t csi_gpio_pin_initialize(int32_t gpio_pin, gpio_event_cb_t cb_event)
{
    uint32_t base = 0u;
    uint32_t irq = 0u;
    void *handler;

    if (gpio_pin < 0 || gpio_pin >= CONFIG_GPIO_NUM) {
        return NULL;
    }

    target_gpio_init(gpio_pin, &base, &irq, &handler);

    dw_gpio_pin_priv_t *gpio_pin_priv = &(gpio_pin_handle[gpio_pin]);

    gpio_pin_priv->pinidx = gpio_pin;
    gpio_pin_priv->cb = cb_event;

    gpio_pin_priv->irq = irq;
    gpio_pin_priv->base = base;

    tls_gpio_isr_register((enum tls_io_name)gpio_pin, gpio_irq_callback, gpio_pin_priv);

    drv_irq_register(gpio_pin_priv->irq, handler);
    drv_irq_enable(gpio_pin_priv->irq);

    return gpio_pin_priv;
}

/**
  \brief       De-initialize GPIO pin handle. stops operation and releases the software resources used by the handle
  \param[in]   handle   gpio pin handle to operate.
  \return      error code
*/
int32_t csi_gpio_pin_uninitialize(gpio_pin_handle_t handle)
{
    if (!handle)
        return ERR_GPIO(DRV_ERROR_PARAMETER);

    dw_gpio_pin_priv_t *gpio_pin_priv = (dw_gpio_pin_priv_t *)handle;
    tls_gpio_isr_register((enum tls_io_name)gpio_pin_priv->pinidx, NULL, NULL);
    drv_irq_disable(gpio_pin_priv->irq);
    drv_irq_unregister(gpio_pin_priv->irq);
    gpio_pin_priv->cb = NULL;

    return 0;
}

/**
  \brief       control gpio power.
  \param[in]   idx  gpio index.
  \param[in]   state   power state.\ref csi_power_stat_e.
  \return      error code
*/
int32_t csi_gpio_power_control(gpio_pin_handle_t handle, csi_power_stat_e state)
{
    return ERR_GPIO(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       config pin mode
  \param[in]   pin       gpio pin handle to operate.
  \param[in]   mode      \ref gpio_mode_e
  \return      error code
*/
int32_t csi_gpio_pin_config_mode(gpio_pin_handle_t handle,
                                 gpio_mode_e mode)
{
    if (!handle)
        return ERR_GPIO(DRV_ERROR_PARAMETER);

    dw_gpio_pin_priv_t *gpio_pin_priv = handle;

    if (GPIO_MODE_PULLNONE == mode)
        tls_gpio_cfg_attr(gpio_pin_priv->pinidx, WM_GPIO_ATTR_FLOATING);
    else if (GPIO_MODE_PULLUP == mode)
        tls_gpio_cfg_attr(gpio_pin_priv->pinidx, WM_GPIO_ATTR_PULLHIGH);
    else if (GPIO_MODE_PULLDOWN == mode)
        tls_gpio_cfg_attr(gpio_pin_priv->pinidx, WM_GPIO_ATTR_PULLLOW);
    else if (GPIO_MODE_OPEN_DRAIN == mode)
        tls_io_cfg_set(gpio_pin_priv->pinidx, WM_IO_OPTION5);
    else if (GPIO_MODE_PUSH_PULL == mode)
        return ERR_GPIO(DRV_ERROR_UNSUPPORTED);
    else
        return ERR_GPIO(DRV_ERROR_PARAMETER);

    return 0;
}
/**
  \brief       config pin direction
  \param[in]   pin       gpio pin handle to operate.
  \param[in]   dir       \ref gpio_direction_e
  \return      error code
*/
int32_t csi_gpio_pin_config_direction(gpio_pin_handle_t handle, gpio_direction_e dir)
{
    if (!handle)
        return ERR_GPIO(DRV_ERROR_PARAMETER);

    dw_gpio_pin_priv_t *gpio_pin_priv = handle;

    if (GPIO_DIRECTION_INPUT == dir)
        tls_gpio_cfg_dir(gpio_pin_priv->pinidx, WM_GPIO_DIR_INPUT);
    else if (GPIO_DIRECTION_OUTPUT == dir)
        tls_gpio_cfg_dir(gpio_pin_priv->pinidx, WM_GPIO_DIR_OUTPUT);
    else
        return ERR_GPIO(DRV_ERROR_PARAMETER);

    return 0;
}

/**
  \brief       config pin
  \param[in]   handle       gpio pin handle to operate.
  \param[in]   mode      \ref gpio_mode_e
  \param[in]   dir       \ref gpio_direction_e
  \return      error code
*/
int32_t csi_gpio_pin_config(gpio_pin_handle_t handle,
                            gpio_mode_e mode,
                            gpio_direction_e dir)
{
    if (!handle)
        return ERR_GPIO(DRV_ERROR_PARAMETER);

    int32_t ret;

    ret = csi_gpio_pin_config_mode(handle, mode);
    if (ret)
        return ret;

    ret = csi_gpio_pin_config_direction(handle, dir);
    if (ret)
        return ret;

    return 0;
}

/**
  \brief       Set one or zero to the selected GPIO pin.
  \param[in]   handle       gpio pin handle to operate.
  \param[in]   value     the value to be set
  \return      error code
*/
int32_t csi_gpio_pin_write(gpio_pin_handle_t handle, bool value)
{
    if (!handle)
        return ERR_GPIO(DRV_ERROR_PARAMETER);

    dw_gpio_pin_priv_t *gpio_pin_priv = handle;

    tls_gpio_write(gpio_pin_priv->pinidx, value);

    return 0;

}

/**
  \brief       Get the value of  selected GPIO pin.
  \param[in]   handle       gpio pin handle to operate.
  \param[out]  value     buf to store the pin value
  \return      error code
*/
int32_t csi_gpio_pin_read(gpio_pin_handle_t handle, bool *value)
{
    if (!handle || !value)
        return ERR_GPIO(DRV_ERROR_PARAMETER);

    dw_gpio_pin_priv_t *gpio_pin_priv = handle;

    *value = tls_gpio_read(gpio_pin_priv->pinidx);

    return 0;
}

/**
  \brief       set GPIO interrupt mode.
  \param[in]   handle       gpio pin handle to operate.
  \param[in]   mode      the irq mode to be set
  \param[in]   enable    the enable flag
  \return      error code
*/
int32_t csi_gpio_pin_set_irq(gpio_pin_handle_t handle, gpio_irq_mode_e mode, bool enable)
{
    if (!handle)
        return ERR_GPIO(DRV_ERROR_PARAMETER);

    dw_gpio_pin_priv_t *gpio_pin_priv = handle;

    if (enable)
    {
        tls_clr_gpio_irq_status(gpio_pin_priv->pinidx);

        if (GPIO_IRQ_MODE_RISING_EDGE == mode)
            tls_gpio_irq_enable(gpio_pin_priv->pinidx, WM_GPIO_IRQ_TRIG_RISING_EDGE);
        else if (GPIO_IRQ_MODE_FALLING_EDGE == mode)
            tls_gpio_irq_enable(gpio_pin_priv->pinidx, WM_GPIO_IRQ_TRIG_FALLING_EDGE);
        else if (GPIO_IRQ_MODE_DOUBLE_EDGE == mode)
            tls_gpio_irq_enable(gpio_pin_priv->pinidx, WM_GPIO_IRQ_TRIG_DOUBLE_EDGE);
        else if (GPIO_IRQ_MODE_LOW_LEVEL == mode)
            tls_gpio_irq_enable(gpio_pin_priv->pinidx, WM_GPIO_IRQ_TRIG_LOW_LEVEL);
        else if (GPIO_IRQ_MODE_HIGH_LEVEL == mode)
            tls_gpio_irq_enable(gpio_pin_priv->pinidx, WM_GPIO_IRQ_TRIG_HIGH_LEVEL);
        else
            return ERR_GPIO(DRV_ERROR_PARAMETER);
    }
    else
    {
        tls_gpio_irq_disable(gpio_pin_priv->pinidx);
    }

    return 0;
}

