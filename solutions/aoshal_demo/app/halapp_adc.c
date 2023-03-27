/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#if !defined(CONFIG_CHIP_D1)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <drv/pin.h>
#include <aos/aos.h>
#include "aos/hal/adc.h"
#include "app_main.h"
#include "soc.h"

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

int hal_adc_out_signal_demo(uint8_t gpio_pin)
{
    uint32_t  val[ADC_NUM];
    adc_dev_t adc_dev;
    uint8_t   port;
    int       ret;
    int       cnt;

    printf("hal_adc_value_get test start\n");

    uint32_t pin_func = get_adc_pin_func(gpio_pin, adc_pinmap);
    csi_pin_set_mux(gpio_pin, pin_func);

    port = get_adc_channel(gpio_pin, adc_pinmap);
    printf("hal_adc_out_signal_demo on port %d\r\n", port);

    adc_dev.port = port;
    ret = hal_adc_init(&adc_dev);

    if (ret == 0) {
        printf("hal_adc_init ok...\n");
    } else {
        printf("hal_adc_init error\n");
        return -1;;
    }

    cnt = 5;
    while (cnt-- > 0) {

        memset(val, 0, sizeof(val));

        for (int i = 0; i < ADC_NUM; i++) {
            ret = hal_adc_value_get(&adc_dev, &val[i], 1000);

            if (ret == 0) {
                printf("adc port %d val[%d] = 0x%03x\r\n", adc_dev.port, i, val[i]);
            } else {
                printf("%s: get value error, ret %d\r\n", __func__, ret);
                return -1;
            }
        }

        aos_msleep(500);
    }

    hal_adc_finalize(&adc_dev);

    printf("hal_adc_out_signal_demo on port %d end\r\n", port);

    return 0;
}

int hal_adc_out_multiple_demo(uint8_t gpio_pin)
{
    uint32_t  val[ADC_NUM];
    adc_dev_t adc_dev;
    uint8_t   port;
    int       ret;
    int       cnt;

    printf("hal_adc_value_multiple_get test start\n");

    uint32_t pin_func = get_adc_pin_func(gpio_pin, adc_pinmap);
    csi_pin_set_mux(gpio_pin, pin_func);

    port = get_adc_channel(gpio_pin, adc_pinmap);
    printf("hal_adc_out_multiple_demo on port %d\r\n", port);

    adc_dev.port = port;
    ret = hal_adc_init(&adc_dev);

    if (ret == 0) {
        printf("hal_adc_init ok...\n");
    } else {
        printf("hal_adc_init error\n");
        return -1;
    }

    cnt = 5;
    while (cnt-- > 0) {

        memset(val, 0, sizeof(val));

        ret = hal_adc_value_multiple_get(&adc_dev, val, ADC_NUM, 1000);

        if (ret == 0) {
            for (int i = 0; i < ADC_NUM; i++) {
                printf("adc port %d val[%d] = 0x%03x\r\n", adc_dev.port, i, val[i]);
            }
        } else {
            printf("%s: get value error, ret %d\r\n", __func__, ret);
            return -1;
        }

        aos_msleep(500);
    }

    hal_adc_finalize(&adc_dev);

    printf("hal_adc_out_multiple_demo on port %d end\r\n", port);

    return 0;
}

void task_adc_multiple_pin0(void *priv)
{
    uint8_t port = *((uint8_t *)priv);

    hal_adc_out_multiple_demo(port);
    aos_msleep(500);
}

void task_adc_multiple_pin1(void *priv)
{
    uint8_t port = *((uint8_t *)priv);

    hal_adc_out_multiple_demo(port);
    aos_msleep(500);
}

int hal_task_adc_multiple_demo(uint8_t gpio_pin0, uint8_t gpio_pin1)
{
    aos_task_t task;

    aos_task_new_ext(&task, "adc_pin0", task_adc_multiple_pin0, (void *)&gpio_pin0, 4096, 32);
    aos_task_new_ext(&task, "adc_pin1", task_adc_multiple_pin1, (void *)&gpio_pin1, 4096, 32);

    return 0;
}

void task_adc_signal_pin0(void *priv)
{
    uint8_t port = *((uint8_t *)priv);

    hal_adc_out_signal_demo(port);
    aos_msleep(500);
}

void task_adc_signal_pin1(void *priv)
{
    uint8_t port = *((uint8_t *)priv);

    hal_adc_out_signal_demo(port);
    aos_msleep(500);
}

int hal_task_adc_signal_demo(uint8_t gpio_pin0, uint8_t gpio_pin1)
{
    aos_task_t task;

    aos_task_new_ext(&task, "adc_port0", task_adc_signal_pin0, (void *)&gpio_pin0, 4096, 32);
    aos_task_new_ext(&task, "adc_port1", task_adc_signal_pin1, (void *)&gpio_pin1, 4096, 32);

    return 0;
}

#endif
