/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <aos/cli.h>
#include <aos/hal/gpio.h>

int32_t gpio_output_set(int port, int val)
{
    int32_t    ret  = -1;
    gpio_dev_t gpio = { 0, OUTPUT_PUSH_PULL, NULL };

    gpio.port = port;

    ret = hal_gpio_init(&gpio);

    if (ret == 0) {
        if (val) {
            ret = hal_gpio_output_high(&gpio);
        } else {
            ret = hal_gpio_output_low(&gpio);
        }
    }

    hal_gpio_finalize(&gpio);

    return ret;
}

int32_t gpio_input_get(int port)
{
    int32_t    ret   = -1;
    gpio_dev_t gpio  = { 0, INPUT_PULL_DOWN, NULL };
    uint32_t   value = 0;

    gpio.port = port;

    ret = hal_gpio_init(&gpio);
    if (ret == 0) {
        ret = hal_gpio_input_get(&gpio, &value);
    }

    hal_gpio_finalize(&gpio);

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
