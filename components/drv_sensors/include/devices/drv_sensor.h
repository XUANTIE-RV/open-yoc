/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __DRV_SENSOR_H__
#define __DRV_SENSOR_H__

#include <stdint.h>
#include <devices/sensor.h>
#ifdef __cplusplus
extern "C" {
#endif
#include <devices/hal/sensor_impl.h>

#define DHT11_DEV_NAME "DHT11"
#define DHT22_DEV_NAME "DHT22"

typedef struct _sensor_temphumi sensor_dht11_t;
typedef struct _sensor_temphumi sensor_dht22_t;

void sensor_dht11_register(sensor_pin_config_t *config, int idx);
void sensor_dht22_register(sensor_pin_config_t *config, int idx);
void sensor_light_csky_register(void *config, int idx);
void sensor_mpu6050_register(void *config, int idx);
void sensor_sht20_register(void *config, int idx);
void sensor_simulate_register(void *config, int idx);


#ifdef __cplusplus
}
#endif

#endif