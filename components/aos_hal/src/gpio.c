/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

#include "drv/gpio.h"
#include "pin_name.h"
#include "pinmux.h"
#include "aos/hal/gpio.h"
#include "string.h"

static uint32_t pin_out[4];
static struct {
    gpio_irq_handler_t gpio_cb[144];
    void *arg[144];
} priv_gpio_cb;

static void gpio_cb(int idx)
{
    if (priv_gpio_cb.gpio_cb[idx] != NULL) {
        priv_gpio_cb.gpio_cb[idx](priv_gpio_cb.arg[idx]);
    }
}

int32_t hal_gpio_init(gpio_dev_t *gpio)
{
    if (gpio == NULL || ANALOG_MODE == gpio->config) {
        return -1;
    }

    gpio_pin_handle_t handle = csi_gpio_pin_initialize(gpio->port, NULL);
    if (handle == NULL) {
        return -1;
    }

    gpio->priv = (void *)handle;
    gpio_mode_e mode = GPIO_MODE_PULLNONE;
    gpio_direction_e dir = GPIO_DIRECTION_INPUT;

    if (gpio->config == INPUT_PULL_UP) {
        mode = GPIO_MODE_PULLUP;
    } else if (gpio->config == INPUT_PULL_DOWN) {
        mode = GPIO_MODE_PULLDOWN;
    } else if (gpio->config == INPUT_HIGH_IMPEDANCE) {
        mode = GPIO_MODE_OPEN_DRAIN;
    } else if (gpio->config == OUTPUT_PUSH_PULL) {
        mode = GPIO_MODE_PUSH_PULL;
        dir = GPIO_DIRECTION_OUTPUT;
    } else if (gpio->config == OUTPUT_OPEN_DRAIN_NO_PULL) {
        mode = GPIO_MODE_PULLNONE;
        dir = GPIO_DIRECTION_OUTPUT;
    } else if (gpio->config == OUTPUT_OPEN_DRAIN_PULL_UP) {
        mode = GPIO_MODE_PULLUP;
        dir = GPIO_DIRECTION_OUTPUT;
    }

    csi_gpio_pin_config_direction(handle, dir);
    csi_gpio_pin_config_mode(handle, mode);

    return 0;
}

int32_t hal_gpio_output_high(gpio_dev_t *gpio)
{
    if (gpio == NULL) {
        return -1;
    }

    uint8_t offset =  gpio->port / 32;
    pin_out[offset] |= 1 << (gpio->port % 32);

    return csi_gpio_pin_write((gpio_pin_handle_t)gpio->priv, 1);
}

int32_t hal_gpio_output_low(gpio_dev_t *gpio)
{
    if (gpio == NULL) {
        return -1;
    }

    uint8_t offset =  gpio->port / 32;
    pin_out[offset] &= ~(1 << (gpio->port % 32));

    return csi_gpio_pin_write((gpio_pin_handle_t)gpio->priv, 0);
}

int32_t hal_gpio_output_toggle(gpio_dev_t *gpio)
{
    if (gpio == NULL) {
        return -1;
    }

    uint8_t offset =  gpio->port / 32;

    if (pin_out[offset] & (1 << (gpio->port % 32))) {
        pin_out[offset] &= ~(1 <<(gpio->port % 32));
        return csi_gpio_pin_write((gpio_pin_handle_t)gpio->priv, 0);
    }

    pin_out[offset] |= 1 << (gpio->port % 32);
    return csi_gpio_pin_write((gpio_pin_handle_t)gpio->priv, 1);
}

int32_t hal_gpio_input_get(gpio_dev_t *gpio, uint32_t *value)
{
    if (gpio == NULL) {
        return -1;
    }

    return csi_gpio_pin_read((gpio_pin_handle_t)gpio->priv, (bool *)value);
}

int32_t hal_gpio_enable_irq(gpio_dev_t *gpio, gpio_irq_trigger_t trigger,
                            gpio_irq_handler_t handler, void *arg)
{
    if (gpio == NULL) {
        return -1;
    }

    priv_gpio_cb.gpio_cb[gpio->port] = handler;
    priv_gpio_cb.arg[gpio->port] = arg;

    gpio_pin_handle_t handle = csi_gpio_pin_initialize(gpio->port, gpio_cb);
    if (handle == NULL) {
        return -1;
    }

    gpio_irq_mode_e mode;

    if (trigger == IRQ_TRIGGER_RISING_EDGE) {
        mode = GPIO_IRQ_MODE_RISING_EDGE;
    } else if (trigger == IRQ_TRIGGER_FALLING_EDGE) {
        mode = GPIO_IRQ_MODE_FALLING_EDGE;
    } else if (trigger == IRQ_TRIGGER_BOTH_EDGES) {
        mode = GPIO_IRQ_MODE_DOUBLE_EDGE;
    } else {
        return -1;
    }

    return csi_gpio_pin_set_irq(handle, mode, 1);
}

int32_t hal_gpio_disable_irq(gpio_dev_t *gpio)
{
    if (gpio == NULL) {
        return -1;
    }

    priv_gpio_cb.gpio_cb[gpio->port] = NULL;
    priv_gpio_cb.arg[gpio->port] = NULL;
    gpio_pin_handle_t handle = (gpio_pin_handle_t)gpio->priv;

    int ret = csi_gpio_pin_set_irq(handle, IRQ_TRIGGER_RISING_EDGE, 0);
    ret += csi_gpio_pin_set_irq(handle, GPIO_IRQ_MODE_FALLING_EDGE, 0);
    ret += csi_gpio_pin_set_irq(handle, GPIO_IRQ_MODE_DOUBLE_EDGE, 0);

    return ret;
}

int32_t hal_gpio_clear_irq(gpio_dev_t *gpio)
{
    if (gpio == NULL) {
        return -1;
    }

    return 0;
}

int32_t hal_gpio_finalize(gpio_dev_t *gpio)
{
    if (gpio == NULL) {
        return -1;
    }

    return csi_gpio_pin_uninitialize((gpio_pin_handle_t)gpio->priv);
}
