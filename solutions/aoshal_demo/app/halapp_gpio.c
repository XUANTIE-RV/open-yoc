/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <aos/aos.h>
#include "aos/hal/gpio.h"

int hal_gpio_out_demo(int port)
{
    int        cnt = 20;
    gpio_dev_t gpio_out;
    int        flag = 0;

    printf("hal_gpio_out_demo start\r\n");

    gpio_out.port   = port;
    gpio_out.config = OUTPUT_OPEN_DRAIN_NO_PULL;
    gpio_out.priv   = NULL;
    hal_gpio_init(&gpio_out);

    while ((cnt--) > 0) {
        if (flag) {
            hal_gpio_output_low(&gpio_out);
        } else {
            hal_gpio_output_high(&gpio_out);
        }
        flag ^= 1;
        aos_msleep(1000);
    }
    hal_gpio_finalize(&gpio_out);

    printf("hal_gpio_out_demo end\r\n");

    return 0;
}

volatile static bool intr_flag = false;
static void hal_gpio_int_fun(void *priv)
{
    intr_flag = true;
}

int hal_gpio_int_demo(int port, int trigger_method)
{
    gpio_dev_t gpio_int;

    printf("hal_gpio_int_demo start\r\n");

    gpio_int.port   = port;
    gpio_int.config = IRQ_MODE;
    gpio_int.priv   = NULL;
    hal_gpio_init(&gpio_int);
    hal_gpio_enable_irq(&gpio_int, trigger_method, hal_gpio_int_fun, NULL);

    while (1) {
        if (intr_flag) {
            printf("GPIO input test successful\n");
            intr_flag = false;
            break;
        }
    }

    hal_gpio_disable_irq(&gpio_int);

    printf("hal_gpio_int_demo end\r\n");

    return 0;
}
