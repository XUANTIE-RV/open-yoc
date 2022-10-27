/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef HAL_SENSOR_IMPL_H
#define HAL_SENSOR_IMPL_H

#include <stdint.h>

#include <devices/driver.h>
#include <devices/sensor.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sensor_pin_config {
    int pin;
} sensor_pin_config_t;

typedef struct sensor_driver {
    driver_t drv;
    int (*fetch)(aos_dev_t *dev);
    int (*getvalue)(aos_dev_t *dev, void *value, size_t size);
} sensor_driver_t;

#ifdef __cplusplus
}
#endif

#endif
