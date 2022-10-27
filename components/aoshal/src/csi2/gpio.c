/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */
#include "drv/gpio.h"
#include "drv/gpio_pin.h"
#include "drv/pin.h"
#include "aos/hal/gpio.h"
#include "string.h"

#include <soc.h>
#include <drv/common.h>
#include <ulog/ulog.h>

#define TAG "GPIO"

csi_gpio_pin_t gpio_pin_dev[64];
static struct {
    gpio_irq_handler_t gpio_cb[144];
    void *arg[144];
} priv_gpio_cb;

static void gpio_cb(csi_gpio_pin_t *gpio, void *arg)
{
    if (priv_gpio_cb.gpio_cb[*(uint8_t*)arg] != NULL) {
        priv_gpio_cb.gpio_cb[*(uint8_t*)arg](priv_gpio_cb.arg[*(uint8_t*)arg]);
    }
}

int32_t hal_gpio_init(gpio_dev_t *gpio)
{
    csi_error_t ret = CSI_OK;

    if (gpio == NULL || ANALOG_MODE == gpio->config) {
        return -1;
    }

    if(gpio == NULL||gpio->config==ANALOG_MODE){
        return -1;
    }

    ret = csi_pin_set_mux(gpio->port,  PIN_FUNC_GPIO);

    if (ret != CSI_OK) {
        LOGE(TAG, "csi_pin_set_mux error");
        return -1;
    }

    if(csi_gpio_pin_init(&gpio_pin_dev[gpio->port],gpio->port) == -1){
        LOGE(TAG, "csi_gpio_pin_init error");
        return -1;
    }

    if (gpio->config == INPUT_PULL_UP) {
        ret = csi_gpio_pin_mode(&gpio_pin_dev[gpio->port],GPIO_MODE_PULLUP);

        if (ret != CSI_OK) {
            LOGE(TAG, "csi_gpio_pin_mode error");
            return -1;
        }
    } else if (gpio->config == INPUT_PULL_DOWN) {
        ret = csi_gpio_pin_mode(&gpio_pin_dev[gpio->port],GPIO_MODE_PULLDOWN);

        if (ret != CSI_OK) {
            LOGE(TAG, "csi_gpio_pin_mode error");
            return -1;
        }
    } else if (gpio->config == INPUT_HIGH_IMPEDANCE) {
        ret = csi_gpio_pin_mode(&gpio_pin_dev[gpio->port],GPIO_MODE_OPEN_DRAIN);

        if (ret != CSI_OK) {
            LOGE(TAG, "csi_gpio_pin_mode error");
            return -1;
        }
    } else if (gpio->config == OUTPUT_PUSH_PULL) {
        ret = csi_gpio_pin_mode(&gpio_pin_dev[gpio->port],GPIO_MODE_PUSH_PULL);

        if (ret != CSI_OK) {
            LOGE(TAG, "csi_gpio_pin_mode error");
            return -1;
        }

        ret = csi_gpio_pin_dir(&gpio_pin_dev[gpio->port],GPIO_DIRECTION_OUTPUT);

        if (ret != CSI_OK) {
            LOGE(TAG, "csi_gpio_pin_dir error");
            return -1;
        }
    } else if (gpio->config == OUTPUT_OPEN_DRAIN_NO_PULL) {
        ret = csi_gpio_pin_mode(&gpio_pin_dev[gpio->port],GPIO_MODE_PULLNONE);

        if (ret != CSI_OK) {
            LOGE(TAG, "csi_gpio_pin_mode error");
            return -1;
        }

        ret = csi_gpio_pin_dir(&gpio_pin_dev[gpio->port],GPIO_DIRECTION_OUTPUT);

        if (ret != CSI_OK) {
            LOGE(TAG, "csi_gpio_pin_dir error");
            return -1;
        }

    } else if (gpio->config == OUTPUT_OPEN_DRAIN_PULL_UP) {
        ret = csi_gpio_pin_mode(&gpio_pin_dev[gpio->port],GPIO_MODE_PULLUP);

        if (ret != CSI_OK) {
            LOGE(TAG, "csi_gpio_pin_mode error");
            return -1;
        }

        ret = csi_gpio_pin_dir(&gpio_pin_dev[gpio->port],GPIO_DIRECTION_OUTPUT);

        if (ret != CSI_OK) {
            LOGE(TAG, "csi_gpio_pin_dir error");
            return -1;
        }
    } else if(gpio->config == IRQ_MODE){
        ret = csi_gpio_pin_mode(&gpio_pin_dev[gpio->port],GPIO_MODE_PULLNONE);

        if (ret != CSI_OK) {
            LOGE(TAG, "csi_gpio_pin_mode error");
            return -1;
        }

        ret = csi_gpio_pin_dir(&gpio_pin_dev[gpio->port],GPIO_DIRECTION_INPUT);

        if (ret != CSI_OK) {
            LOGE(TAG, "csi_gpio_pin_dir error");
            return -1;
        }
    }

    return 0;
}

int32_t hal_gpio_output_high(gpio_dev_t *gpio)
{
    if (gpio == NULL) {
        return -1;
    }

    csi_gpio_pin_write(&gpio_pin_dev[gpio->port],GPIO_PIN_HIGH);

    return 0;
}

int32_t hal_gpio_output_low(gpio_dev_t *gpio)
{
    if (gpio == NULL) {
        return -1;
    }

    csi_gpio_pin_write(&gpio_pin_dev[gpio->port],GPIO_PIN_LOW);

    return 0;
}

int32_t hal_gpio_output_toggle(gpio_dev_t *gpio)
{
    if (gpio == NULL) {
        return -1;
    }

    csi_gpio_pin_toggle(&gpio_pin_dev[gpio->port]);

    return 0;
}

int32_t hal_gpio_input_get(gpio_dev_t *gpio, uint32_t *value)
{
    if (gpio == NULL) {
        return -1;
    }

    *value = csi_gpio_pin_read(&gpio_pin_dev[gpio->port]);

    return 0;
}

int32_t hal_gpio_enable_irq(gpio_dev_t *gpio, gpio_irq_trigger_t trigger,gpio_irq_handler_t handler, void *arg)
{
    if (gpio == NULL) {
        return -1;
    }

    csi_error_t    ret = CSI_OK;

    priv_gpio_cb.gpio_cb[gpio->port] = handler;
    priv_gpio_cb.arg[gpio->port] = arg;

    ret = csi_gpio_pin_attach_callback(&gpio_pin_dev[gpio->port],gpio_cb,&gpio->port);

    if (ret != CSI_OK) {
        LOGE(TAG, "csi_gpio_pin_attach_callback error");
        return -1;
    }

    if (trigger == IRQ_TRIGGER_RISING_EDGE) {
        ret = csi_gpio_pin_irq_mode(&gpio_pin_dev[gpio->port],GPIO_IRQ_MODE_RISING_EDGE);

        if (ret != CSI_OK) {
            LOGE(TAG, "csi_gpio_pin_irq_mode error");
            return -1;
        }
    } else if (trigger == IRQ_TRIGGER_FALLING_EDGE) {
        ret = csi_gpio_pin_irq_mode(&gpio_pin_dev[gpio->port],GPIO_IRQ_MODE_FALLING_EDGE);

        if (ret != CSI_OK) {
            LOGE(TAG, "csi_gpio_pin_irq_mode error");
            return -1;
        }
    } else if (trigger == IRQ_TRIGGER_BOTH_EDGES) {
        ret = csi_gpio_pin_irq_mode(&gpio_pin_dev[gpio->port],GPIO_IRQ_MODE_BOTH_EDGE);

        if (ret != CSI_OK) {
            LOGE(TAG, "csi_gpio_pin_irq_mode error");
            return -1;
        }
    } else {
        return -1;
    }

    ret = csi_gpio_pin_irq_enable(&gpio_pin_dev[gpio->port],1);

    if (ret != CSI_OK) {
        LOGE(TAG, "csi_gpio_pin_irq_enable error");
        return -1;
    }

    return ret;
}

int32_t hal_gpio_disable_irq(gpio_dev_t *gpio)
{
    if (gpio == NULL) {
        return -1;
    }

    return 0;
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

    csi_gpio_pin_uninit(&gpio_pin_dev[gpio->port]);

    return 0;
}