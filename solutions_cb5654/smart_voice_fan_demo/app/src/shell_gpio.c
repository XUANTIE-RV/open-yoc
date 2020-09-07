/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <pin_name.h>
#include <pinmux.h>
#include <drv/gpio.h>
#include "shell.h"
#include "app_printf.h"

volatile static bool int_flag = 1;

static void gpio_interrupt_handler(int32_t idx)
{
    int_flag = 0;
}

void gpio_output_test(int gpio_id, int val)
{
    gpio_pin_handle_t     handle;

    drv_pinmux_config(gpio_id, PIN_FUNC_GPIO);
    handle = csi_gpio_pin_initialize(gpio_id, NULL);
    csi_gpio_pin_config_mode(handle, GPIO_MODE_PUSH_PULL);
    csi_gpio_pin_config_direction(handle, GPIO_DIRECTION_OUTPUT);
    csi_gpio_pin_write(handle, val);
    csi_gpio_pin_uninitialize(handle);
}

void gpio_interrupt_test(int gpio_id)
{
    gpio_pin_handle_t     handle;

    drv_pinmux_config(gpio_id, PIN_FUNC_GPIO);
    handle = csi_gpio_pin_initialize(gpio_id, gpio_interrupt_handler);
    csi_gpio_pin_config_mode(handle, GPIO_MODE_PULLNONE);
    csi_gpio_pin_config_direction(handle, GPIO_DIRECTION_INPUT);
    csi_gpio_pin_set_irq(handle, GPIO_IRQ_MODE_RISING_EDGE, 1);

    while (int_flag);

    int_flag = 1;
    csi_gpio_pin_uninitialize(handle);
}

bool gpio_input_test(int gpio_id)
{
    gpio_pin_handle_t     handle;
    bool val = 0;

    drv_pinmux_config(gpio_id, PIN_FUNC_GPIO);
    handle = csi_gpio_pin_initialize(gpio_id, NULL);
    csi_gpio_pin_config_mode(handle, GPIO_MODE_PULLNONE);
    csi_gpio_pin_config_direction(handle, GPIO_DIRECTION_INPUT);
    csi_gpio_pin_read(handle, &val);
    csi_gpio_pin_uninitialize(handle);

    return val;
}

static void cmd_app_func(int argc, char *argv[])
{
    int cmd_err = 0;
    if (argc < 3) {
        cmd_err = 1;
    }

    if (cmd_err == 0) {
        if (strcmp(argv[1], "set") == 0) {
            if (argc == 4) {
                gpio_output_test(atoi(argv[2]), atoi(argv[3]));
                LOGI("set gpio %d val=%d\n", atoi(argv[2]), atoi(argv[3]));
            } else {
                cmd_err = 1;
            }
        } else if (strcmp(argv[1], "get") == 0) {
            bool val = gpio_input_test(atoi(argv[2]));
            LOGI("get gpio %d val=%d\n", atoi(argv[2]), val);
        } else if (strcmp(argv[1], "irq") == 0) {
            gpio_interrupt_test(atoi(argv[2]));
            LOGI("gpio %d irq\n", atoi(argv[2]));
        }
    }

    if (cmd_err) {
        LOGD("usage:\n");
        LOGD("\tgpio set pinid 0/1\n");
        LOGD("\tgpio get pinid\n");
        LOGD("\tgpio irq pinid\n");
    }
}

void shell_reg_cmd_gpio(void)
{
    shell_cmd_t cmd_info = {"gpio", "gpio set/get/irq", cmd_app_func};

    shell_register_command(&cmd_info);
}
