/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <drv/adc.h>
#include <drv/gpio.h>

#include <errno.h>

#include "hal/battery_impl.h"

#define BATTERY_ADC_CONTINOUS_CHANNEL   5
#define BATTERY_ADC_CON_DATA_NUM        1
#define BATTERY_ADC_CHANNEL_IDX         0

typedef struct battery_dev {
    aos_dev_t           device;
    adc_handle_t    adc_handle;
    gpio_pin_handle_t gpio_handle;
    int             data;
} battery_dev_t;

static battery_event_cb_t g_batery_event_cb = NULL;

static aos_dev_t *battery_simulate_init(driver_t *drv, void *config, int id)
{
    battery_dev_t *dev = (battery_dev_t *)device_new(drv, sizeof(battery_dev_t), id);

    if (dev) {
        dev->device.config = config;
    }

    return (aos_dev_t *)dev;
}

#define battery_simulate_uninit device_free

static int battery_simulate_lpm(aos_dev_t *dev, int state)
{
    return 0;
}

static void battery_simulate_event_cb(int32_t idx)
{
    if (g_batery_event_cb) {
        g_batery_event_cb(REMOVED);
    }
}

static int battery_simulate_config(aos_dev_t *dev, uint32_t gpio, gpio_event_cb_t cb)
{
#if 0
    battery_dev_t *battery = (battery_dev_t *)dev;

    int ret;

    battery->gpio_handle = csi_gpio_pin_initialize(gpio, cb);

    if (battery->gpio_handle == NULL) {
        return -1;
    }

    ret = csi_gpio_pin_config(battery->gpio_handle, GPIO_MODE_PULLNONE, GPIO_DIRECTION_INPUT);

    if (ret) {
        return -1;
    }

    ret = csi_gpio_pin_set_irq(battery->gpio_handle, GPIO_IRQ_MODE_RISING_EDGE, 1);

    if (ret) {
        return -1;
    }
#endif
    return 0;
}

static int battery_simulate_open(aos_dev_t *dev)
{
    int ret;
    battery_dev_t *battery = (battery_dev_t *)dev;
#if 0
    adc_conf_t sconfig;
    uint32_t ch_array[1] = {BATTERY_ADC_CONTINOUS_CHANNEL};

    sconfig.mode = ADC_CONTINUOUS;
    sconfig.trigger = 0;
    sconfig.intrp_mode = 0;
    sconfig.channel_array = ch_array;
    sconfig.channel_nbr = 1;
    sconfig.conv_cnt = BATTERY_ADC_CON_DATA_NUM;

    battery->adc_handle = drv_adc_initialize(BATTERY_ADC_CHANNEL_IDX, NULL);

    if (!battery->adc_handle) {
        printf("adc initial fail\r\n");
        return -1;
    }

    ret = drv_adc_config(battery->adc_handle, &sconfig);

    if (ret < 0) {
        printf("adc config fail\r\n");
        return -1;
    }
#endif
    ret = battery_simulate_config(dev, ((battery_pin_config_t *)battery->device.config)->pin, battery_simulate_event_cb);

    if (ret < 0) {
        return -1;
    }

    return 0;
}

static int battery_simulate_close(aos_dev_t *dev)
{
#if 0
    battery_dev_t *battery = (battery_dev_t *)dev;
    drv_adc_uninitialize(battery->adc_handle);
#endif
    return 0;
}

static int battery_simulate_attr_fetch(aos_dev_t *dev, hal_battery_attr_t attr)
{
    battery_dev_t *battery = (battery_dev_t *)dev;
    static int data_change = 4095;
    static uint8_t chang_time = 0;
    static uint8_t data_up = 0;

    int total = 0;
#if 0
    int i, ret;
    static uint32_t recv_data[BATTERY_ADC_CON_DATA_NUM];

    memset(recv_data, 0, sizeof(uint32_t) * BATTERY_ADC_CON_DATA_NUM);

    ret = drv_adc_start(battery->adc_handle);

    if (ret < 0) {
        printf("adc start fail\r\n");
        return -1;
    }

    ret = drv_adc_read(battery->adc_handle, &recv_data[0], BATTERY_ADC_CON_DATA_NUM);

    if (ret < 0) {
        printf("adc read fail\r\n");
    }

    for (i = 0; i < BATTERY_ADC_CON_DATA_NUM; i++) {
        total += recv_data[i];
    }
#endif

    total = data_change;

    battery->data = total;

    chang_time++;

    if (chang_time >= 5) {
        if (data_up == 0) {
            if (data_change <= 1) {
                data_up = 1;
            }

            data_change -= 500;
        } else {
            if (data_change >= 4094) {
                data_up = 0;
            }

            data_change += 500;
        }

        chang_time = 0;
    }

#if 0
    ret = drv_adc_stop(battery->adc_handle);

    if (ret < 0) {
        printf("adc stop fail\r\n");
        return -1;
    }
#endif

    return 0;

}

static int battery_simulate_attr_get(aos_dev_t *dev, hal_battery_attr_t attr, void *value, size_t size)
{
    battery_dev_t *battery = (battery_dev_t *)dev;

    if (attr == VOLTAGE) {
        if (size < sizeof(battery_voltage_t)) {
            return -EINVAL;
        }

        ((battery_voltage_t *)value)->volt = (((battery->data / BATTERY_ADC_CON_DATA_NUM) & 0xFFF) * 3300) / 4096;

    }

    if (attr == CURRENT) {
        if (size < sizeof(battery_current_t)) {
            return -EINVAL;
        }

        //TODO
    }

    return 0;
}

static battery_driver_t battery_driver = {
    .drv = {
        .name   = "battery_simulate",
        .type   = "battery",
        .init   = battery_simulate_init,
        .uninit = battery_simulate_uninit,
        .lpm    = battery_simulate_lpm,
        .open   = battery_simulate_open,
        .close  = battery_simulate_close,
    },
    .fetch      = battery_simulate_attr_fetch,
    .getvalue   = battery_simulate_attr_get,
};


void battery_simulate_register(battery_pin_config_t *config, int idx)
{
    driver_register(&battery_driver.drv, (void *)config, idx);

    if (config) {
        g_batery_event_cb = config->event_cb;
    }
}
