/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <yoc/button.h>

#include "internal.h"

#define b_gpio_param(b) ((gpio_button_param_t*)(b->param))

static button_t *button_gpio_find(int pin_id)
{
    button_t *b = NULL;

    slist_for_each_entry(&g_button_srv.button_head, b, button_t, next) {
        button_ops_t *ops = &gpio_ops;

        if ((b->ops == ops) && b_gpio_param(b)->pin_id == pin_id) {
            return b;
        }
    }

    return NULL;
}

void pin_event(rvm_dev_t *dev, void *arg)
{
    button_t *button = NULL;
    int idx = (size_t)arg;

    button = button_gpio_find(idx);
    rvm_hal_gpio_pin_irq_enable(b_gpio_param(button)->pin_hdl, false);

    if (button != NULL && (b_gpio_param(button)->pin_hdl != NULL)) {
        button_irq(button);
    }
}

static int gpio_irq_disable(button_t *button)
{
    if (b_gpio_param(button)->active_level == LOW_LEVEL) {
        rvm_hal_gpio_pin_irq_enable(b_gpio_param(button)->pin_hdl, false);
    } else {
        rvm_hal_gpio_pin_irq_enable(b_gpio_param(button)->pin_hdl, false);
    }
    return 0;
}

static int gpio_irq_enable(button_t *button)
{
    rvm_hal_gpio_pin_irq_enable(b_gpio_param(button)->pin_hdl, true);
    if (b_gpio_param(button)->active_level == LOW_LEVEL) {
        rvm_hal_gpio_pin_set_irq_mode(b_gpio_param(button)->pin_hdl, RVM_GPIO_IRQ_MODE_FALLING_EDGE);
    } else {
        rvm_hal_gpio_pin_set_irq_mode(b_gpio_param(button)->pin_hdl, RVM_GPIO_IRQ_MODE_RISING_EDGE);
    }
    rvm_hal_gpio_pin_attach_callback(b_gpio_param(button)->pin_hdl, pin_event, (void *)(size_t)b_gpio_param(button)->pin_id);
    return 0;
}

static int gpio_pin_read(button_t *button)
{
    uint32_t val = 0;
    int is_pressed = 0;

    rvm_hal_gpio_pin_read(b_gpio_param(button)->pin_hdl, (rvm_hal_gpio_pin_data_t *)&val);
    if (val == b_gpio_param(button)->active_level) {
        is_pressed = 1;
    }

    return is_pressed;
}

static int gpio_pin_init(button_t *button)
{
    int pin_id = b_gpio_param(button)->pin_id;
    rvm_gpio_pin_drv_register(pin_id);
    rvm_dev_t *gpio_pin = rvm_hal_gpio_pin_open_by_pin_name("gpio_pin", pin_id);
    if (!gpio_pin) {
        return -1;
    }
    rvm_hal_gpio_mode_t mode;
    if (b_gpio_param(button)->active_level == LOW_LEVEL) {
        mode = RVM_GPIO_MODE_PULLUP;
    } else {
        mode = RVM_GPIO_MODE_PULLDOWN;
    }
    rvm_hal_gpio_pin_set_mode(gpio_pin, mode);
    b_gpio_param(button)->pin_hdl = gpio_pin;
    gpio_irq_enable(button);
    return 0;
}

static int gpio_pin_deinit(button_t *button)
{
    rvm_hal_gpio_pin_close(b_gpio_param(button)->pin_hdl);
    b_gpio_param(button)->pin_hdl = NULL;
    return 0;
}

button_ops_t gpio_ops = {
    .init = gpio_pin_init,
    .deinit = gpio_pin_deinit,
    .read = gpio_pin_read,
    .irq_disable = gpio_irq_disable,
    .irq_enable = gpio_irq_enable,
};
