/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <aos/aos.h>
#include <soc.h>

#include <drv/pin.h>
#include <devices/device.h>
#include <devices/driver.h>
#include <devices/gpiopin.h>
#include <devices/devicelist.h>

int hal_gpio_out_demo(int port)
{
    int        cnt = 5;
    int        flag = 0;
    int        ret = -1;

    printf("hal_gpio_out_demo start\r\n");

    csi_pin_set_mux(port,  PIN_FUNC_GPIO);

    rvm_gpio_pin_drv_register(port);
    rvm_dev_t *gpio_dev = NULL;
    char filename[16];
    sprintf(filename, "gpio_pin%d", port);

    gpio_dev = rvm_hal_gpio_pin_open(filename);

    ret = rvm_hal_gpio_pin_set_direction(gpio_dev, RVM_GPIO_DIRECTION_OUTPUT);
    if (ret != 0) {
        printf("rvm_hal_gpio_pin_set_direction fail !\r\n");
        return -1;
    }

    ret = rvm_hal_gpio_pin_set_mode(gpio_dev, RVM_GPIO_MODE_PULLNONE);
    if (ret != 0) {
        printf("rvm_hal_gpio_pin_set_mode fail !\r\n");
        return -1;
    }

    /* output high or low */
    while ((cnt--) > 0) {
        if (flag) {
            ret = rvm_hal_gpio_pin_write(gpio_dev, RVM_GPIO_PIN_LOW);
            if (ret != 0) {
                printf("rvm_hal_gpio_pin_write fail !\r\n");
                return -1;
            }

        } else {
            ret = rvm_hal_gpio_pin_write(gpio_dev, RVM_GPIO_PIN_HIGH);
            if (ret != 0) {
                printf("rvm_hal_gpio_pin_write fail !\r\n");
                return -1;
            }
        }
        flag ^= 1;
        aos_msleep(1000);
    }

    /* rvm_hal_gpio_pin_output_toggle */
    cnt = 5;
    while ((cnt--) > 0) {
        ret = rvm_hal_gpio_pin_output_toggle(gpio_dev);
        if (ret != 0) {
            printf("rvm_hal_gpio_pin_output_toggle fail !\r\n");
            return -1;
        }

        aos_msleep(3000);
    }

    rvm_hal_gpio_pin_close(gpio_dev);

    printf("hal_gpio_out_demo end\r\n");

    return 0;
}

volatile static bool intr_flag = false;
static void hal_gpio_int_fun(rvm_dev_t *dev, void *arg)
{

    rvm_hal_gpio_pin_data_t value[1];
    intr_flag = true;

    int ret = rvm_hal_gpio_pin_read(dev, value);
    if (ret != 0) {
        printf("rvm_hal_gpio_pin_read fail !\r\n");
        return;
    }

    printf("gpio value is: %d\n", value[0]);

    return;
}

int hal_gpio_int_demo(int port, int trigger_method)
{
    int ret = -1;
    
    printf("hal_gpio_int_demo start\r\n");

    csi_pin_set_mux(port,  PIN_FUNC_GPIO);

    rvm_gpio_pin_drv_register(port);
    rvm_dev_t *gpio_dev = NULL;
    char filename[16];
    sprintf(filename, "gpio_pin%d", port);

    gpio_dev = rvm_hal_gpio_pin_open(filename);

    ret = rvm_hal_gpio_pin_set_mode(gpio_dev, RVM_GPIO_MODE_PULLNONE);
    if (ret != 0) {
        printf("rvm_hal_gpio_pin_set_mode fail !\r\n");
        return -1;
    }

    ret = rvm_hal_gpio_pin_set_direction(gpio_dev, RVM_GPIO_DIRECTION_INPUT);
    if (ret != 0) {
        printf("rvm_hal_gpio_pin_set_direction fail !\r\n");
        return -1;
    }

    ret = rvm_hal_gpio_pin_attach_callback(gpio_dev, hal_gpio_int_fun, NULL);
    if (ret != 0) {
        printf("rvm_hal_gpio_pin_attach_callback fail !\r\n");
        return -1;
    }

    if (trigger_method == 1) {
        ret = rvm_hal_gpio_pin_set_irq_mode(gpio_dev, RVM_GPIO_IRQ_MODE_RISING_EDGE);
        if (ret != 0) {
            printf("rvm_hal_gpio_pin_set_irq_mode fail !\r\n");
            return -1;
        }

    } else if (trigger_method == 2) {
        ret = rvm_hal_gpio_pin_set_irq_mode(gpio_dev, RVM_GPIO_IRQ_MODE_FALLING_EDGE);
        if (ret != 0) {
            printf("rvm_hal_gpio_pin_set_irq_mode fail !\r\n");
            return -1;
        }

    }
    
    ret = rvm_hal_gpio_pin_irq_enable(gpio_dev, 1);
    if (ret != 0) {
        printf("rvm_hal_gpio_pin_irq_enable fail !\r\n");
        return -1;
    }

    while (1) {
        if (intr_flag) {
            printf("GPIO input test successful\n");
            intr_flag = false;
            break;
        }
    }

    ret = rvm_hal_gpio_pin_irq_enable(gpio_dev, 0);
    if (ret != 0) {
        printf("rvm_hal_gpio_pin_irq_enable fail !\r\n");
        return -1;
    }

    rvm_hal_gpio_pin_close(gpio_dev);

    printf("hal_gpio_int_demo end\r\n");

    return 0;
}
