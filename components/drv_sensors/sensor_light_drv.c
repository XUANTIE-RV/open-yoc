/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     sensor_dht11.c
 * @brief    sensor dht11 is using dht11 to get data
 * @version  V1.0
 * @date     05. June 2017
 ******************************************************************************/

#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include <drv/adc.h>
#include <errno.h>

#include <devices/hal/sensor_impl.h>

#define SENSOR_ADC_CONTINOUS_CHANNEL    0
#define SENSOR_ADC_CON_DATA_NUM         16
#define SENSOR_ADC_CHANNEL_IDX          0

typedef struct sensor_dev {
    aos_dev_t           device;
    adc_handle_t    handle;
    int             data;
} sensor_dev_t;

static aos_dev_t *yoc_sensor_light_init(driver_t *drv, void *config, int id)
{
    sensor_dev_t *dev = (sensor_dev_t*)device_new(drv, sizeof(sensor_dev_t), id);

    dev->device.config = config;

    return (aos_dev_t*)dev;
}

#define yoc_sensor_light_uninit device_free

static int yoc_sensor_light_lpm(aos_dev_t *dev, int state)
{
    return 0;
}

static int yoc_sensor_light_open(aos_dev_t *dev)
{
    sensor_dev_t * sensor = (sensor_dev_t *)dev;

    int ret;
    adc_conf_t sconfig;
    uint32_t ch_array[1] = {SENSOR_ADC_CONTINOUS_CHANNEL};

    sconfig.mode = ADC_CONTINUOUS;
    sconfig.trigger = 0;
    sconfig.intrp_mode = 0;
    sconfig.channel_array = ch_array;
    sconfig.channel_nbr = 1;
    sconfig.conv_cnt = SENSOR_ADC_CON_DATA_NUM;

    sensor->handle = drv_adc_initialize(SENSOR_ADC_CHANNEL_IDX, NULL);

    if (!sensor->handle) {
        printf("adc initial fail\r\n");
        return -1;
    }

    ret = drv_adc_config(sensor->handle, &sconfig);

    if (ret < 0) {
        printf("adc config fail\r\n");
        return -1;
    }

    return 0;
}

static int yoc_sensor_light_close(aos_dev_t *dev)
{
    sensor_dev_t *sensor = (sensor_dev_t *)dev;

    drv_adc_uninitialize(sensor->handle);

    return 0;
}

static int yoc_sensor_light_fetch(aos_dev_t *dev)
{
    sensor_dev_t *sensor = (sensor_dev_t *)dev;

    int i, ret;
    int total = 0;
    static uint32_t recv_data[SENSOR_ADC_CON_DATA_NUM];

    for (i = 0; i < SENSOR_ADC_CON_DATA_NUM; i++) {
        recv_data[i] = 0;
    }

    ret = drv_adc_start(sensor->handle);

    if (ret < 0) {
        printf("adc start fail\r\n");
        return -1;
    }

    ret = drv_adc_read(sensor->handle, &recv_data[0], SENSOR_ADC_CON_DATA_NUM);

    if (ret < 0) {
        printf("adc read fail\r\n");
    }

    for (i = 0; i < SENSOR_ADC_CON_DATA_NUM; i++) {
        total += recv_data[i];
    }

    sensor->data = (total / SENSOR_ADC_CON_DATA_NUM);

    ret = drv_adc_stop(sensor->handle);

    if (ret < 0) {
        printf("adc stop fail\r\n");
        return -1;
    }

    return 0;
}

static int yoc_sensor_light_get(aos_dev_t *dev, void *value, size_t size)
{
    sensor_dev_t *sensor = (sensor_dev_t *)dev;

    if (size < sizeof (sensor_light_t)) {
        return -EINVAL;
    }

    ((sensor_light_t *)value)->status = 100 - ((sensor->data * 100) / 4096);

    return 0;
}

static sensor_driver_t sensor_driver = {
    .drv = {
        .name   = "light",
        .type   = "sensor",
        .init   = yoc_sensor_light_init,
        .uninit = yoc_sensor_light_uninit,
        .lpm    = yoc_sensor_light_lpm,
        .open   = yoc_sensor_light_open,
        .close  = yoc_sensor_light_close,
    },
    .fetch      = yoc_sensor_light_fetch,
    .getvalue   = yoc_sensor_light_get,
};

void sensor_light_csky_register(void *config, int idx)
{
    driver_register(&sensor_driver.drv, config, idx);
}
