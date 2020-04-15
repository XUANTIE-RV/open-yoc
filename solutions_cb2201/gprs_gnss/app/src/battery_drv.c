/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <drv_adc.h>
#include <drv/gpio.h>
#include <aos/log.h>
#include <aos/aos.h>
#include <devices/driver.h>
#include <devices/hal/battery_impl.h>

#include <errno.h>
#include "battery_api.h"

#define TAG "battery"

extern void udelay(uint32_t us);

#define BATTERY_ADC_CON_DATA_NUM        16
#define BATTERY_ADC_IDX                 0

typedef struct battery_dev {
    aos_dev_t           device;
    adc_handle_t    adc_handle;
    gpio_pin_handle_t gpio_handle;
    int             data;
} battery_dev_t;

static battery_event_cb_t g_batery_event_cb = NULL;

static aos_dev_t *battery_fetch_init(driver_t *drv, void *config, int id)
{
    battery_dev_t *dev = (battery_dev_t *)device_new(drv, sizeof(battery_dev_t), id);
    dev->device.config = config;
    return (aos_dev_t *)dev;
}

#define battery_fetch_uninit device_free

static int battery_fetch_lpm(aos_dev_t *dev, int state)
{
    return 0;
}

static void battery_fetch_event_cb(int32_t idx)
{
    if (g_batery_event_cb) {
        g_batery_event_cb(REMOVED);
    }
}

static int battery_config_removepin(aos_dev_t *dev, uint32_t gpio, gpio_event_cb_t cb)
{
    battery_dev_t *battery = (battery_dev_t *)dev;

    int ret;

    battery->gpio_handle = csi_gpio_pin_initialize(gpio, cb);

    if (battery->gpio_handle == NULL) {
        return -1;
    }

    ret = csi_gpio_pin_config(battery->gpio_handle, GPIO_MODE_PULLNONE, GPIO_DIRECTION_INPUT);

    if (ret != 0) {
        return -1;
    }

    ret = csi_gpio_pin_set_irq(battery->gpio_handle, GPIO_IRQ_MODE_RISING_EDGE, 1);

    if (ret != 0) {
        return -1;
    }
    return 0;
}

static int battery_fetch_open(aos_dev_t *dev)
{
    int ret;
    battery_dev_t *battery = (battery_dev_t *)dev;
    adc_conf_t sconfig;
    uint32_t ch_array[1] = {0};

    ch_array[0] = ((battery_pin_config_t *)battery->device.config)->adc_channel;

    sconfig.mode = ADC_CONTINUOUS;
    sconfig.trigger = 0;
    sconfig.intrp_mode = 0;
    sconfig.channel_array = ch_array;
    sconfig.channel_nbr = 1;
    sconfig.conv_cnt = BATTERY_ADC_CON_DATA_NUM;

    battery->adc_handle = drv_adc_initialize(BATTERY_ADC_IDX, NULL);
    if (!battery->adc_handle) {
        LOGE(TAG, "adc initial fail");
        return -1;
    }

    ret = drv_adc_config(battery->adc_handle, &sconfig);
    if (ret < 0) {
        LOGE(TAG, "adc config fail");
        return -1;
    }

    if (((battery_pin_config_t *)battery->device.config)->pin > 0) {
        ret = battery_config_removepin(dev, ((battery_pin_config_t *)battery->device.config)->pin, battery_fetch_event_cb);
        if (ret < 0) {
            LOGE(TAG, "battery config fail");
            return -1;
        }
    }

    return 0;
}

static int battery_fetch_close(aos_dev_t *dev)
{
    battery_dev_t *battery = (battery_dev_t *)dev;
    drv_adc_uninitialize(battery->adc_handle);
    return 0;
}

static int battery_fetch_attr_fetch(aos_dev_t *dev, hal_battery_attr_t attr)
{
    battery_dev_t *battery = (battery_dev_t *)dev;
    int total = 0;
    int i, ret;
    static uint32_t recv_data[BATTERY_ADC_CON_DATA_NUM];

    memset(recv_data, 0, sizeof(recv_data));

    ret = drv_adc_start(battery->adc_handle);
    if (ret < 0) {
        LOGE(TAG, "adc start fail");
        return -1;
    }

    ret = drv_adc_read(battery->adc_handle, recv_data, BATTERY_ADC_CON_DATA_NUM);
    if (ret < 0) {
        LOGE(TAG, "adc read fail - %d", ret);
        return -1;
    }

    for (i = 0; i < BATTERY_ADC_CON_DATA_NUM; i++) {
        total += recv_data[i];
    }

    battery->data = total;

    ret = drv_adc_stop(battery->adc_handle);
    if (ret < 0) {
        LOGE(TAG, "adc stop fail");
        return -1;
    }
    return 0;
}

static int battery_fetch_attr_get(aos_dev_t *dev, hal_battery_attr_t attr, void *value, size_t size)
{
    battery_dev_t *battery = (battery_dev_t *)dev;

    if (attr == VOLTAGE) {
        if (size < sizeof(battery_voltage_t)) {
            LOGE(TAG, "size err");
            return -1;
        }

        ((battery_voltage_t *)value)->volt = (((battery->data / BATTERY_ADC_CON_DATA_NUM) & 0xFFF) * 3300) / 4096;
    }

    if (attr == CURRENT) {
        if (size < sizeof(battery_current_t)) {
            LOGE(TAG, "size err");
            return -1;
        }

        //TODO
    }

    return 0;
}

static battery_driver_t battery_fetch_driver = {
    .drv = {
        .name   = "battery_fetch",
        .type   = "battery",
        .init   = battery_fetch_init,
        .uninit = battery_fetch_uninit,
        .lpm    = battery_fetch_lpm,
        .open   = battery_fetch_open,
        .close  = battery_fetch_close,
    },
    .fetch      = battery_fetch_attr_fetch,
    .getvalue   = battery_fetch_attr_get,
};


void battery_fetch_register(battery_pin_config_t *config, int idx)
{
    driver_register(&battery_fetch_driver.drv, (void *)config, idx);

    if (config) {
        g_batery_event_cb = config->event_cb;
    }
}
