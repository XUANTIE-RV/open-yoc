/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <aos/cli.h>
#include <devices/gpiopin.h>
#include <devices/devicelist.h>

int32_t gpio_output_set(int port, int val)
{
    int32_t    ret  = -1;
    rvm_dev_t *gpio_pin;
    rvm_gpio_pin_drv_register(port);
    gpio_pin = rvm_hal_gpio_pin_open_by_pin_name("gpio_pin", port);
    if (gpio_pin) {
        rvm_hal_gpio_pin_set_mode(gpio_pin, RVM_GPIO_MODE_PUSH_PULL);
        if (val) {
            ret = rvm_hal_gpio_pin_write(gpio_pin, RVM_GPIO_PIN_HIGH);
        } else {
            ret = rvm_hal_gpio_pin_write(gpio_pin, RVM_GPIO_PIN_LOW);
        }
        rvm_hal_gpio_pin_close(gpio_pin);
    }

    return ret;
}

int32_t gpio_input_get(int port)
{
    int32_t    ret   = -1;
    uint32_t   value = 0;
    rvm_dev_t *gpio_pin;
    rvm_gpio_pin_drv_register(port);
    gpio_pin = rvm_hal_gpio_pin_open_by_pin_name("gpio_pin", port);
    if (gpio_pin) {
        rvm_hal_gpio_pin_set_mode(gpio_pin, RVM_GPIO_MODE_PULLDOWN);
        ret = rvm_hal_gpio_pin_read(gpio_pin, (rvm_hal_gpio_pin_data_t *)&value);
        rvm_hal_gpio_pin_close(gpio_pin);
    }

    return (ret == 0) ? (int32_t)value : -1;
}

static void cmd_gpio_func(char *wbuf, int wbuf_len, int argc, char **argv)
{
    int32_t ret = -1;

    if (argc < 3) {
        printf("usage:\n");
        printf("\tgpio set pinid 0|1\n");
        printf("\tgpio get pinid\n");
        return;
    }

    if (strcmp(argv[1], "set") == 0) {
        if (argc == 4) {
            ret = gpio_output_set(atoi(argv[2]), atoi(argv[3]));
            printf("set gpio %d val=%d\n", atoi(argv[2]), atoi(argv[3]));
        }
    } else if (strcmp(argv[1], "get") == 0) {
        ret = gpio_input_get(atoi(argv[2]));
        printf("get gpio %d val=%d\n", atoi(argv[2]), ret);
    } else {
        ;
    }
}

void cli_reg_cmd_gpio(void)
{
    static const struct cli_command cmd_info = { "gpio", "gpio test", cmd_gpio_func };

    aos_cli_register_command(&cmd_info);
}
