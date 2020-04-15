/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     csi_sensor_sht20.c
 * @brief    sensor sht20 is using csi to get data
 * @version  V1.0
 * @date     05. June 2017
 ******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <errno.h>

#include <devices/hal/sensor_impl.h>

typedef struct sensor_dev {
    aos_dev_t    device;
    uint32_t data1;
    uint32_t data2;
} sensor_dev_t;


static aos_dev_t *yoc_sensor_simu_init(driver_t *drv,void *config, int id)
{
    sensor_dev_t *dev = (sensor_dev_t*)device_new(drv, sizeof(sensor_dev_t), id);
    dev->device.config = config;

    return (aos_dev_t*)dev;
}

#define yoc_sensor_simu_uninit device_free

static int yoc_sensor_simu_lpm(aos_dev_t *dev, int state)
{
    return 0;
}

static int yoc_sensor_simu_open(aos_dev_t *dev)
{

    return 0;
}

static int yoc_sensor_simu_close(aos_dev_t *dev)
{

    return 0;
}

static int yoc_sensor_simu_fetch(aos_dev_t *dev)
{
    sensor_dev_t *sensor = (sensor_dev_t*)dev;

    sensor->data1 = 500 + rand() % 400;
    sensor->data2 = 200 + rand() % 100;

    return 0;
}

static int yoc_sensor_simu_get(aos_dev_t *dev, void *value, size_t size)
{
    sensor_dev_t *sensor = (sensor_dev_t*)dev;

    if (size < sizeof (sensor_simulate_t)) {
        return -EINVAL;
    }

    ((sensor_simulate_t *)value)->rh  = sensor->data1;
    ((sensor_simulate_t *)value)->degree  = sensor->data2;

    return 0;
}

static sensor_driver_t sensor_driver = {
    .drv = {
        .name   = "simulate",
        .type   = "sensor",
        .init   = yoc_sensor_simu_init,
        .uninit = yoc_sensor_simu_uninit,
        .lpm    = yoc_sensor_simu_lpm,
        .open   = yoc_sensor_simu_open,
        .close  = yoc_sensor_simu_close,
    },
    .fetch      = yoc_sensor_simu_fetch,
    .getvalue   = yoc_sensor_simu_get,
};

void sensor_simulate_register(void *config, int idx)
{
    driver_register(&sensor_driver.drv, config, idx);
}
