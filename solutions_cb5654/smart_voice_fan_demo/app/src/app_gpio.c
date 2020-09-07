/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include "soc.h"
#include <drv/gpio.h>
#include "board_config.h"
#include <pinmux.h>
#include "app_printf.h"


static volatile bool int_flag = 1;

static void gpio_interrupt_handler(int32_t idx)
{
    (void)idx;
    int_flag = 0;
}

static void gpio_pin_init(void)
{
    drv_pinmux_config(TEST_GPIO_PIN, TEST_GPIO_PIN_FUNC);
}

void gpio_test_func(void)
{
    bool pin_value = false;
    gpio_pin_handle_t pin = NULL;

    gpio_pin_init();
    pin = csi_gpio_pin_initialize(TEST_GPIO_PIN, gpio_interrupt_handler);

    csi_gpio_pin_config_mode(pin, GPIO_MODE_PULLNONE);
    csi_gpio_pin_config_direction(pin, GPIO_DIRECTION_INPUT);

    csi_gpio_pin_set_irq(pin, GPIO_IRQ_MODE_RISING_EDGE, 1);

    csi_gpio_pin_read(pin, &pin_value);
    LOGD("csi_gpio_pin_read value:%d\n", pin_value);

    csi_gpio_pin_write(pin, pin_value);

    while (int_flag);
    LOGD("gpio_test_func int_flag:%d\n", int_flag);
    int_flag = 1;

    csi_gpio_pin_uninitialize(pin);
}
