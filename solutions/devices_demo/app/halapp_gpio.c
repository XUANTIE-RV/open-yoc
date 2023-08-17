/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <aos/aos.h>
#include <debug/dbg.h>
#include <soc.h>

#include <drv/pin.h>
#include <devices/device.h>
#include <devices/driver.h>
#include <devices/gpiopin.h>
#include <devices/devicelist.h>
#include "app_main.h"

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

    if (gpio_dev == NULL) {
        printf("open %s failed.\r\n", filename);
        return -1;
    }
    printf("open %s ok.\n", filename);

    ret = rvm_hal_gpio_pin_set_mode(gpio_dev, RVM_GPIO_MODE_PULLNONE);
    if (ret != 0) {
        printf("rvm_hal_gpio_pin_set_mode fail !\r\n");
        goto failure;
    }

    ret = rvm_hal_gpio_pin_set_direction(gpio_dev, RVM_GPIO_DIRECTION_OUTPUT);
    if (ret != 0) {
        printf("rvm_hal_gpio_pin_set_direction fail !\r\n");
        goto failure;
    }

    printf("output data, please wait seconds...\n");
    /* output high or low */
    while ((cnt--) > 0) {
        if (flag) {
            ret = rvm_hal_gpio_pin_write(gpio_dev, RVM_GPIO_PIN_LOW);
            if (ret != 0) {
                printf("rvm_hal_gpio_pin_write fail !\r\n");
                goto failure;
            }
        } else {
            ret = rvm_hal_gpio_pin_write(gpio_dev, RVM_GPIO_PIN_HIGH);
            if (ret != 0) {
                printf("rvm_hal_gpio_pin_write fail !\r\n");
                goto failure;
            }
        }
        flag ^= 1;
        aos_msleep(500);
    }

#if !defined(CONFIG_CHIP_D1)
    /* rvm_hal_gpio_pin_output_toggle */
    cnt = 5;
    while ((cnt--) > 0) {
        ret = rvm_hal_gpio_pin_output_toggle(gpio_dev);
        if (ret != 0) {
            printf("rvm_hal_gpio_pin_output_toggle fail !\r\n");
            goto failure;
        }
        aos_msleep(3000);
    }
#endif
    rvm_hal_gpio_pin_close(gpio_dev);

    printf("hal_gpio_out_demo end\r\n");

    return 0;

failure:
    rvm_hal_gpio_pin_close(gpio_dev);
    return -1;
}

volatile static bool intr_flag = false;
static void hal_gpio_in_fun(rvm_dev_t *dev, void *arg)
{
    rvm_hal_gpio_pin_data_t value[1];
    intr_flag = true;

    int ret = rvm_hal_gpio_pin_read(dev, value);
    if (ret != 0) {
        printk("rvm_hal_gpio_pin_read fail !\r\n");
        return;
    }

    printk("gpio value is: %d\n", value[0]);

    return;
}

int hal_gpio_in_demo(int port, int trigger_method)
{
    int ret = -1;

#if defined(CONFIG_CHIP_CH2601)
    printf("ch2601 not support edge trigger.\n");
    return 0;
#endif

    printf("hal_gpio_in_demo start\r\n");

    csi_pin_set_mux(port, PIN_FUNC_GPIO);

    rvm_gpio_pin_drv_register(port);
    rvm_dev_t *gpio_dev = NULL;
    char filename[16];
    sprintf(filename, "gpio_pin%d", port);

    gpio_dev = rvm_hal_gpio_pin_open(filename);

    ret = rvm_hal_gpio_pin_set_mode(gpio_dev, RVM_GPIO_MODE_PULLNONE);
    if (ret != 0) {
        printf("rvm_hal_gpio_pin_set_mode fail !\r\n");
        goto failure;
    }

    ret = rvm_hal_gpio_pin_set_direction(gpio_dev, RVM_GPIO_DIRECTION_INPUT);
    if (ret != 0) {
        printf("rvm_hal_gpio_pin_set_direction fail !\r\n");
        goto failure;
    }

    ret = rvm_hal_gpio_pin_attach_callback(gpio_dev, hal_gpio_in_fun, NULL);
    if (ret != 0) {
        printf("rvm_hal_gpio_pin_attach_callback fail !\r\n");
        goto failure;
    }

    if (trigger_method == 1) {
        ret = rvm_hal_gpio_pin_set_irq_mode(gpio_dev, RVM_GPIO_IRQ_MODE_RISING_EDGE);
        if (ret != 0) {
            printf("rvm_hal_gpio_pin_set_irq_mode fail !\r\n");
            goto failure;
        }
    } else if (trigger_method == 2) {
        ret = rvm_hal_gpio_pin_set_irq_mode(gpio_dev, RVM_GPIO_IRQ_MODE_FALLING_EDGE);
        if (ret != 0) {
            printf("rvm_hal_gpio_pin_set_irq_mode fail !\r\n");
            goto failure;
        }
    }
    
    ret = rvm_hal_gpio_pin_irq_enable(gpio_dev, 1);
    if (ret != 0) {
        printf("rvm_hal_gpio_pin_irq_enable fail !\r\n");
        goto failure;
    }

    printf("wait GPIO interrupt...\n");
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
        goto failure;
    }

    rvm_hal_gpio_pin_close(gpio_dev);
    printf("hal_gpio_in_demo end\r\n");
    return 0;

failure:
    rvm_hal_gpio_pin_close(gpio_dev);
    return -1;
}


#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int devfs_gpio_out_demo(int port)
{
    int ret = -1;
    int cnt = 5;
    int flag = 0;

    printf("devfs_gpio_out_demo start...\n");

    csi_pin_set_mux(port,  PIN_FUNC_GPIO);

    rvm_gpio_pin_drv_register(port);

    char gpiodev[16];
    sprintf(gpiodev, "/dev/gpio_pin%d", port);

    int fd = open(gpiodev, O_RDWR);
    printf("open %s fd:%d\n", gpiodev, fd);
    if (fd < 0) {
        printf("open %s failed. fd:%d\n", gpiodev, fd);
        return -1;
    }

    rvm_hal_gpio_mode_t mode = RVM_GPIO_MODE_PULLNONE;
    ret = ioctl(fd, GPIOPIN_IOC_SET_MODE, &mode);
    if (ret < 0) {
        printf("GPIOPIN_IOC_SET_MODE fail !\n");
        goto failure;
    }

    rvm_hal_gpio_dir_t dir = RVM_GPIO_DIRECTION_OUTPUT;
    ret = ioctl(fd, GPIOPIN_IOC_SET_DIRECTION, &dir);
    if (ret < 0) {
        printf("GPIOPIN_IOC_SET_DIRECTION fail !\n");
        goto failure;
    }

    printf("output data, please wait seconds...\n");
    /* output high or low */
    while ((cnt--) > 0) {
        if (flag) {
            rvm_hal_gpio_pin_data_t value = RVM_GPIO_PIN_LOW;
            ret = ioctl(fd, GPIOPIN_IOC_WRITE, &value);
            if (ret < 0) {
                printf("GPIOPIN_IOC_WRITE fail !\n");
                goto failure;
            }

        } else {
            rvm_hal_gpio_pin_data_t value = RVM_GPIO_PIN_HIGH;
            ret = ioctl(fd, GPIOPIN_IOC_WRITE, &value);
            if (ret < 0) {
                printf("GPIOPIN_IOC_WRITE fail !\n");
                goto failure;
            }
        }
        flag ^= 1;
        aos_msleep(1000);
    }

#if !defined(CONFIG_CHIP_D1)
    /* GPIOPIN_IOC_OUTPUT_TOGGLE */
    cnt = 5;
    while ((cnt--) > 0) {
        ret = ioctl(fd, GPIOPIN_IOC_OUTPUT_TOGGLE);
        if (ret < 0) {
            printf("GPIOPIN_IOC_OUTPUT_TOGGLE fail !\n");
            goto failure;
        }
        aos_msleep(3000);
    }
#endif

    close(fd);
    printf("devfs_gpio_out_demo end\n");
    return 0;
failure:
    close(fd);
    return -1;
}

volatile static bool devfs_intr_flag = false;
static rvm_gpiopin_dev_msg_t msg_gpio_input;
static int fd_gpio_input;
static void devfs_gpio_in_fun(rvm_dev_t *dev, void *arg)
{
    devfs_intr_flag = true;

    return;
}

int devfs_gpio_in_demo(int port, int trigger_method)
{
    int ret = -1;

#if defined(CONFIG_CHIP_CH2601)
    printf("ch2601 not support edge trigger.\n");
    return 0;
#endif

    printf("devfs_gpio_in_demo start\r\n");

    csi_pin_set_mux(port,  PIN_FUNC_GPIO);

    rvm_gpio_pin_drv_register(port);

    char gpiodev[16];
    sprintf(gpiodev, "/dev/gpio_pin%d", port);

    fd_gpio_input = open(gpiodev, O_RDWR);
    printf("open %s fd:%d\n", gpiodev, fd_gpio_input);
    if (fd_gpio_input < 0) {
        printf("open %s failed. fd:%d\n", gpiodev, fd_gpio_input);
        return -1;
    }

    rvm_hal_gpio_mode_t mode = RVM_GPIO_MODE_PULLNONE;
    ret = ioctl(fd_gpio_input, GPIOPIN_IOC_SET_MODE, &mode);
    if (ret < 0) {
        printf("GPIOPIN_IOC_SET_MODE fail !\n");
        goto failure;
    }

    rvm_hal_gpio_dir_t dir = RVM_GPIO_DIRECTION_INPUT;
    ret = ioctl(fd_gpio_input, GPIOPIN_IOC_SET_DIRECTION, &dir);
    if (ret < 0) {
        printf("GPIOPIN_IOC_SET_DIRECTION fail !\n");
        goto failure;
    }

    msg_gpio_input.callback = devfs_gpio_in_fun;
    msg_gpio_input.arg = NULL;

    ret = ioctl(fd_gpio_input, GPIOPIN_IOC_ATTACH_CALLBACK, &msg_gpio_input);
    if (ret < 0) {
        printf("GPIOPIN_IOC_ATTACH_CALLBACK fail !\n");
        goto failure;
    }

    rvm_hal_gpio_irq_mode_t irq_mode = 0;

    if (trigger_method == 1) {
        irq_mode = RVM_GPIO_IRQ_MODE_RISING_EDGE;

        ret = ioctl(fd_gpio_input, GPIOPIN_IOC_SET_IRQ_MODE, &irq_mode);
        if (ret < 0) {
            printf("GPIOPIN_IOC_SET_IRQ_MODE fail !\n");
            goto failure;
        }

    } else if (trigger_method == 2) {
        irq_mode = RVM_GPIO_IRQ_MODE_FALLING_EDGE;

        ret = ioctl(fd_gpio_input, GPIOPIN_IOC_SET_IRQ_MODE, &irq_mode);
        if (ret < 0) {
            printf("GPIOPIN_IOC_SET_IRQ_MODE fail !\n");
            goto failure;
        }

    }

    bool enable = 1;

    ret = ioctl(fd_gpio_input, GPIOPIN_IOC_SET_IRQ_ENABLE, &enable);
    if (ret < 0) {
        printf("GPIOPIN_IOC_SET_IRQ_ENABLE fail !\n");
        goto failure;
    }

    rvm_hal_gpio_pin_data_t value[1];

    while (1) {
        if (devfs_intr_flag) {
            printf("GPIO input test successful\n");
            devfs_intr_flag = false;

            msg_gpio_input.value = value;
            int ret = ioctl(fd_gpio_input, GPIOPIN_IOC_READ, &msg_gpio_input);
            if (ret < 0) {
                printf("GPIOPIN_IOC_READ fail !\n");
                goto failure;
            }

            printf("gpio value is: %d\n", value[0]);

            break;
        }
    }

    enable = 0;
    ret = ioctl(fd_gpio_input, GPIOPIN_IOC_SET_IRQ_ENABLE, &enable);
    if (ret < 0) {
        printf("GPIOPIN_IOC_SET_IRQ_ENABLE fail !\n");
        goto failure;
    }

    close(fd_gpio_input);
    printf("devfs_gpio_in_demo end\n");
    return 0;
failure:
    close(fd_gpio_input);
    return -1;
}


#endif