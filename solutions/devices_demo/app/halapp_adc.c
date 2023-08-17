/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#if !defined(CONFIG_CHIP_D1)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <drv/pin.h>
#include <aos/aos.h>
#include "app_main.h"
#include "soc.h"

#include <devices/device.h>
#include <devices/driver.h>
#include <devices/adc.h>
#include <devices/devicelist.h>

/**
 * Convert Analog to Digital, and print out the result
 */

#define ADC_NUM     2

extern const csi_pinmap_t adc_pinmap[];

static uint32_t get_adc_pin_func(uint8_t gpio_pin, const csi_pinmap_t *pinmap)
{
    const csi_pinmap_t *map = pinmap;
    uint32_t ret = 0xFF;

    while ((uint32_t)map->pin_name != 0xFFU) {
        if (map->pin_name == gpio_pin && map->idx == 0) {
            ret = (uint32_t)map->pin_func;
            break;
        }

        map++;
    }

    return ret;
}

static uint32_t get_adc_channel(uint8_t gpio_pin, const csi_pinmap_t *pinmap)
{
    const csi_pinmap_t *map = pinmap;
    uint32_t ret = 0xFFU;

    while ((uint32_t)map->pin_name != 0xFFU) {
        if (map->pin_name == gpio_pin && map->idx == 0) {
            ret = (uint32_t)map->channel;
            break;
        }

        map++;
    }

    return ret;
}

int hal_adc_demo(uint8_t gpio_pin)
{
    uint32_t  val[ADC_NUM];
    uint8_t   port;
    int       ret;
    int       cnt;

    printf("hal adc test start\n");

    uint32_t pin_func = get_adc_pin_func(gpio_pin, adc_pinmap);
    csi_pin_set_mux(gpio_pin, pin_func);

    port = get_adc_channel(gpio_pin, adc_pinmap);
    printf("hal_adc_demo on port %d\r\n", port);

    rvm_adc_drv_register(0);

    rvm_dev_t *adc_dev = NULL;
    char *filename = "adc0";

    adc_dev = rvm_hal_adc_open(filename);

    rvm_hal_adc_config_t config;
    config.sampling_time = 2;
    config.freq = 128;

    ret = rvm_hal_adc_config(adc_dev, &config);

    cnt = 5;
    while (cnt-- > 0) {

        memset(val, 0, sizeof(val));

        for (int i = 0; i < ADC_NUM; i++) {
            ret = rvm_hal_adc_read(adc_dev, port, &val[i], 1000);

            if (ret == 0) {
                printf("adc val[%d] = 0x%03x\r\n", i, val[i]);
            } else {
                printf("%s: get value error, ret %d\r\n", __func__, ret);
                return -1;
            }
        }

        aos_msleep(500);
    }

    rvm_hal_adc_close(adc_dev);

    printf("hal_adc_demo on port %d end\r\n", port);

    return 0;
}

void task_adc_pin0(void *priv)
{
    uint8_t port = *((uint8_t *)priv);

    hal_adc_demo(port);
    aos_msleep(500);
}

void task_adc_pin1(void *priv)
{
    uint8_t port = *((uint8_t *)priv);

    hal_adc_demo(port);
    aos_msleep(500);
}

int hal_task_adc_demo(uint8_t gpio_pin0, uint8_t gpio_pin1)
{
    aos_task_t task;

    aos_task_new_ext(&task, "adc_port0", task_adc_pin0, (void *)&gpio_pin0, 4096, 32);
    aos_task_new_ext(&task, "adc_port1", task_adc_pin1, (void *)&gpio_pin1, 4096, 32);

    return 0;
}

#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int devfs_adc_demo(uint8_t gpio_pin)
{
    uint32_t  val[ADC_NUM];
    uint8_t   port;
    int       ret;
    int       cnt;

    printf("devfs adc test start\n");

    uint32_t pin_func = get_adc_pin_func(gpio_pin, adc_pinmap);
    csi_pin_set_mux(gpio_pin, pin_func);

    port = get_adc_channel(gpio_pin, adc_pinmap);
    printf("devfs_adc_demo on port %d\r\n", port);

    rvm_adc_drv_register(0);

    char *adcdev = "/dev/adc0";

    int fd = open(adcdev, O_RDWR);
    printf("open adc0 fd:%d\n", fd);
    if (fd < 0) {
        printf("open %s failed. fd:%d\n", adcdev, fd);
        return -1;
    }

    rvm_adc_dev_msg_t msg_get_ch;
    msg_get_ch.pin = gpio_pin;

    ret = ioctl(fd, ADC_IOC_GET_PIN2CHANNEL, &msg_get_ch);
    if (ret < 0) {
        printf("ADC_IOC_GET_DEFAULT_CONFIG fail !\n");
        goto failure;
    }

    printf("ADC_IOC_GET_PIN2CHANNEL :%d\n", msg_get_ch.ch);


    rvm_hal_adc_config_t config;

    ret = ioctl(fd, ADC_IOC_GET_DEFAULT_CONFIG, &config);
    if (ret < 0) {
        printf("ADC_IOC_GET_DEFAULT_CONFIG fail !\n");
        goto failure;
    }

    printf(
        "default config:\n"
        "               config.mode          = %d\n"
        "               config.trigger       = %d\n"
        "               config.intrp_mode    = %d\n"
        "               config.sampling_time = %d\n"
        "               config.freq          = %d\n"
        "               config.offset        = %d\n",
        config.mode, config.trigger, config.intrp_mode, config.sampling_time, config.freq, config.offset
    );

    config.sampling_time = 2;
    config.freq = 128;

    ret = ioctl(fd, ADC_IOC_SET_CONFIG, &config);
    if (ret < 0) {
        printf("ADC_IOC_SET_CONFIG fail !\n");
        goto failure;
    }

    cnt = 5;
    rvm_adc_dev_msg_t msg;
    msg.ch = port;
    msg.timeout = 1000;

    printf("ADC_IOC_GET_VALUE test\n");
    while (cnt-- > 0) {

        memset(val, 0, sizeof(val));

        for (int i = 0; i < ADC_NUM; i++) {
            msg.output = &val[i];
            ret = ioctl(fd, ADC_IOC_GET_VALUE, &msg);

            if (ret == 0) {
                printf("adc val[%d] = 0x%03x\r\n", i, val[i]);
            } else {
                printf("%s: get value error, ret %d\r\n", __func__, ret);
                goto failure;
            }
        }

        aos_msleep(500);
    }

    cnt = 5;
    msg.num = ADC_NUM;

    printf("ADC_IOC_GET_MULTIPLE_VALUE test\n");
    while (cnt-- > 0) {

        memset(val, 0, sizeof(val));

        msg.output = val;
        ret = ioctl(fd, ADC_IOC_GET_MULTIPLE_VALUE, &msg);

        if (ret == 0) {
            for (int i = 0; i < ADC_NUM; i++) {
                printf("adc port %d val[%d] = 0x%03x\r\n", msg.ch, i, val[i]);
            }
        } else {
            printf("%s: get value error, ret %d\r\n", __func__, ret);
            goto failure;
        }

        aos_msleep(500);
    }

    close(fd);
    printf("devfs adc demo successfully !\n");
    return 0;
failure:
    close(fd);
    return -1;
}

void task_devfs_adc_pin0(void *priv)
{
    uint8_t port = *((uint8_t *)priv);

    hal_adc_demo(port);
    aos_msleep(500);
}

void task_devfs_adc_pin1(void *priv)
{
    uint8_t port = *((uint8_t *)priv);

    hal_adc_demo(port);
    aos_msleep(500);
}

int devfs_task_adc_demo(uint8_t gpio_pin0, uint8_t gpio_pin1)
{
    aos_task_t task;

    printf("devfs adc multiple task test\n");
    aos_task_new_ext(&task, "adc_port0", task_devfs_adc_pin0, (void *)&gpio_pin0, 4096, 32);
    aos_task_new_ext(&task, "adc_port1", task_devfs_adc_pin1, (void *)&gpio_pin1, 4096, 32);

    return 0;
}
#endif

#endif
