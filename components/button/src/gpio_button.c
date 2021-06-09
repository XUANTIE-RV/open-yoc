/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <yoc/button.h>
#ifdef CONFIG_CSI_V2
#include <soc.h>
#include <drv/pin.h>
#else
#include <pinmux.h>
#endif

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
#ifdef CONFIG_CSI_V2
static void pin_event(csi_gpio_pin_t *pin, void *arg)
#else
static void pin_event(int32_t idx)
#endif
{
    button_t *button = NULL;
#ifdef CONFIG_CSI_V2
    int idx = 0;
    int ctrl_id = pin->gpio->dev.idx;
    idx = pin->pin_idx + (ctrl_id * 32);
#endif
    button = button_gpio_find(idx);

    if (button != NULL) {
        button_irq(button);
    }
}

static int csi_irq_disable(button_t *button)
{
#ifdef CONFIG_CSI_V2
    csi_gpio_pin_irq_enable(&b_gpio_param(button)->pin_hdl, false);
#else
    if (b_gpio_param(button)->active_level == LOW_LEVEL) {
        csi_gpio_pin_set_irq(b_gpio_param(button)->pin_hdl, GPIO_IRQ_MODE_FALLING_EDGE, 0);
    } else {
        csi_gpio_pin_set_irq(b_gpio_param(button)->pin_hdl, GPIO_IRQ_MODE_RISING_EDGE, 0);
    }
#endif
    return 0;
}

static int csi_irq_enable(button_t *button)
{
#ifdef CONFIG_CSI_V2
    if (b_gpio_param(button)->active_level == LOW_LEVEL) {
        csi_gpio_pin_irq_mode(&b_gpio_param(button)->pin_hdl, GPIO_IRQ_MODE_FALLING_EDGE);
    } else {
        csi_gpio_pin_irq_mode(&b_gpio_param(button)->pin_hdl, GPIO_IRQ_MODE_RISING_EDGE);
    }
    csi_gpio_pin_irq_enable(&b_gpio_param(button)->pin_hdl, true);
#else
    if (b_gpio_param(button)->active_level == LOW_LEVEL) {
        csi_gpio_pin_set_irq(b_gpio_param(button)->pin_hdl, GPIO_IRQ_MODE_FALLING_EDGE, 1);
    } else {
        csi_gpio_pin_set_irq(b_gpio_param(button)->pin_hdl, GPIO_IRQ_MODE_RISING_EDGE, 1);
    }
#endif
    return 0;
}

static int csi_pin_read(button_t *button)
{
    bool val;
    bool is_pressed = false;
#ifdef CONFIG_CSI_V2
    int ret = csi_gpio_pin_read(&b_gpio_param(button)->pin_hdl);
    val = ret ? true : false;
#else
    csi_gpio_pin_read(b_gpio_param(button)->pin_hdl, &val);
#endif

    if (val == b_gpio_param(button)->active_level) {
        is_pressed = true;
    }

    return is_pressed;
}

static int csi_pin_init(button_t *button)
{
#ifdef CONFIG_CSI_V2
    memset(&b_gpio_param(button)->pin_hdl, 0, sizeof(b_gpio_param(button)->pin_hdl));
    csi_pin_set_mux(b_gpio_param(button)->pin_id, PIN_FUNC_GPIO);
    csi_gpio_pin_init(&b_gpio_param(button)->pin_hdl, b_gpio_param(button)->pin_id);
    csi_gpio_pin_dir(&b_gpio_param(button)->pin_hdl, GPIO_DIRECTION_INPUT);
    csi_gpio_pin_mode(&b_gpio_param(button)->pin_hdl, GPIO_MODE_PULLUP);
    csi_gpio_pin_debounce(&b_gpio_param(button)->pin_hdl, true);
    csi_gpio_pin_attach_callback(&b_gpio_param(button)->pin_hdl, pin_event, &b_gpio_param(button)->pin_hdl);

    if (b_gpio_param(button)->active_level == LOW_LEVEL) {
        csi_gpio_pin_irq_mode(&b_gpio_param(button)->pin_hdl, GPIO_IRQ_MODE_FALLING_EDGE);
    } else {
        csi_gpio_pin_irq_mode(&b_gpio_param(button)->pin_hdl, GPIO_IRQ_MODE_RISING_EDGE);
    }

    csi_gpio_pin_irq_enable(&b_gpio_param(button)->pin_hdl, true);
#else
    gpio_pin_handle_t pin_hdl;

    drv_pinmux_config(b_gpio_param(button)->pin_id, PIN_FUNC_GPIO);

    pin_hdl = csi_gpio_pin_initialize(b_gpio_param(button)->pin_id, pin_event);
    // csi_gpio_pin_set_evt_priv(pin_hdl, button);
    csi_gpio_pin_config_direction(pin_hdl, GPIO_DIRECTION_INPUT);

    if (b_gpio_param(button)->active_level == LOW_LEVEL) {
        csi_gpio_pin_set_irq(pin_hdl, GPIO_IRQ_MODE_FALLING_EDGE, 0);
        csi_gpio_pin_set_irq(pin_hdl, GPIO_IRQ_MODE_FALLING_EDGE, 1);
    } else {
        csi_gpio_pin_set_irq(pin_hdl, GPIO_IRQ_MODE_RISING_EDGE, 0);
        csi_gpio_pin_set_irq(pin_hdl, GPIO_IRQ_MODE_RISING_EDGE, 1);
    }

    b_gpio_param(button)->pin_hdl = pin_hdl;
#endif
    return 0;
}

static int csi_pin_deinit(button_t *button)
{
#ifdef CONFIG_CSI_V2
    csi_gpio_pin_uninit(&b_gpio_param(button)->pin_hdl);
#else
    csi_gpio_pin_uninitialize(b_gpio_param(button)->pin_hdl);
#endif
    return 0;
}
button_ops_t gpio_ops = {
    .init = csi_pin_init,
    .deinit = csi_pin_deinit,
    .read = csi_pin_read,
    .irq_disable = csi_irq_disable,
    .irq_enable = csi_irq_enable,
};
