/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <yoc_config.h>

#include <stdlib.h>
#include <string.h>

#include <aos/log.h>
#include <aos/kernel.h>

#include <drv/gpio.h>
#include <pin_name.h>
#include <pinmux.h>
#include <devices/dht_sensor.h>

#include "app_main.h"

#define TAG "sensor"

int get_sensor_value(char *name, int *temperature, int *humidity)
{
    aos_dev_t *sensor_dev;
    sensor_dht11_t sval;
    int ret, i;

    sensor_dev = sensor_open_id(name, 0);
    if (sensor_dev == NULL) {
        LOGE(TAG, "no %s driver", name);
        return -1;
    }

    for (i = 0; i < 3; i++) {
        ret = sensor_fetch(sensor_dev);

        if (ret == 0) {
            break;
        }

        //LOGE(TAG, "retry");
        aos_msleep(100);
    }

    if (ret) {
        sensor_close(sensor_dev);
        return -1;
    }

    if (sensor_getvalue(sensor_dev, (void *)&sval, sizeof(sensor_dht11_t)) < 0) {
        sensor_close(sensor_dev);
        return -1;
    }

    sensor_close(sensor_dev);

    *temperature = sval.degree / 10;
    *humidity = sval.rh / 10;

    return 0;
}

void main()
{
    int ret;

    board_yoc_init();
    LOGI(TAG, "dht11 sensor demo\n");

    static sensor_pin_config_t dht11_config = {PA4};
    sensor_dht11_register(&dht11_config, 0);

    int temperature = 0, humidity = 0;

    while (1) {
        ret = get_sensor_value(DHT11_DEV_NAME, &temperature, &humidity);

        if (ret < 0) {
            aos_msleep(2000);
        } else {
            LOGI(TAG, "temperature=%d humidity=%d", temperature, humidity);
            aos_msleep(10000);
        }
    }
}
