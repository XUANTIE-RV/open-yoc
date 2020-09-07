/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>

#include <errno.h>
#include <soc.h>

#include <aos/kernel.h>
#include <drv/gpio.h>
#include <drv/timer.h>

#include <devices/hal/sensor_impl.h>
#include <devices/drv_sensor.h>

#undef LOGE
#define LOGE(...)
#define TAG "dht22"
#define SENSOR_DATA_LEN         40
#define DHT_SENSOR_MDELAY       3 /* ms*/
#define DHT_SENSOR_UDELAY       40 /* us*/
#define DHT_SENSOR_COUNTS       10000


extern void mdelay(uint32_t ms);
extern void udelay(uint32_t us);

typedef enum {
    SENSOR_HUMIDIYT = 0,
    SENSOR_TEMPERATURE,
} sensor_chan_e;

typedef struct sensor_dev {
    aos_dev_t               device;
    gpio_pin_handle_t   handle;
    uint8_t             recv_buf[16];
} sensor_dev_t;

#define sensor(dev) ((sensor_dev_t *)dev)

static int sensor_get_level(aos_dev_t *dev)
{
    bool value;

    csi_gpio_pin_read(sensor(dev)->handle, &value);

    return value;
}

static inline int sensor_read_level(aos_dev_t *dev, int count, int sign_value)
{
    bool value;

    do {
        csi_gpio_pin_read(sensor(dev)->handle, &value);

        if (count-- == 0) {
            //LOGE(TAG, "read timeout");
            return -1;
        }
    } while (value == sign_value);

    return 0;
}

static aos_dev_t *yoc_sensor_dht22_init(driver_t *drv,void *config, int id)
{
    sensor_dev_t *dev = (sensor_dev_t*)device_new(drv, sizeof(sensor_dev_t), id);

    dev->device.config = config;

    return (aos_dev_t*)dev;
}

#define yoc_sensor_dht22_uninit device_free

static int yoc_sensor_dht22_lpm(aos_dev_t *dev, int state)
{
    return 0;
}

static int yoc_sensor_dht22_open(aos_dev_t *dev)
{
    mdelay(DHT_SENSOR_MDELAY);
    sensor(dev)->handle = csi_gpio_pin_initialize(((sensor_pin_config_t *)sensor(dev)->device.config)->pin, NULL);

    return 0;
}

static int yoc_sensor_dht22_close(aos_dev_t *dev)
{
    csi_gpio_pin_uninitialize(sensor(dev)->handle);

    return 0;
}

static int sensor_read_data(aos_dev_t *dev)
{
    int i = 0;
    int ret = -1;
    int j = 0;
    memset(sensor(dev)->recv_buf, 0, sizeof(sensor(dev)->recv_buf));

    ret = csi_gpio_pin_config(sensor(dev)->handle, GPIO_MODE_PUSH_PULL, GPIO_DIRECTION_OUTPUT);

    if (ret < 0) {
        //LOGE(TAG, "set output err");
        return -1;
    }

    /* set gpio low level*/
    ret = csi_gpio_pin_write(sensor(dev)->handle, 0);

    aos_msleep(DHT_SENSOR_MDELAY);

    //uint32_t lpsr = csi_irq_save();
    /* set gpio high level*/
    ret = csi_gpio_pin_write(sensor(dev)->handle, 1);

    udelay(DHT_SENSOR_UDELAY);
    csi_gpio_pin_config(sensor(dev)->handle, GPIO_MODE_PULLUP, GPIO_DIRECTION_INPUT);

    /* wait high level*/
    if (sensor_read_level(dev, DHT_SENSOR_COUNTS, 0) < 0) {
        //LOGE(TAG, "wait high err");
        //csi_irq_restore(lpsr);
        return -1;
    }

    /* set gpio low level*/
    //ret = csi_gpio_pin_write(dev->sensor_dht22_pin, 0);

    /* set gpio mode: input*/
    /* start read bit*/
    for (i = 0; i < SENSOR_DATA_LEN; i++) {
        if (sensor_read_level(dev, DHT_SENSOR_COUNTS, 1) < 0) {
            //LOGE(TAG, "wait data low err, %d", i);
            //csi_irq_restore(lpsr);
            return -1;
        }

        if (sensor_read_level(dev, DHT_SENSOR_COUNTS, 0) < 0) {
            //LOGE(TAG, "wait data high err");
            //csi_irq_restore(lpsr);
            return -1;
        }

        udelay(40);

        if (sensor_get_level(dev) == 0) {
            sensor(dev)->recv_buf[j] = sensor(dev)->recv_buf[j] << 1;
        } else {
            sensor(dev)->recv_buf[j] = (sensor(dev)->recv_buf[j] << 1) | 1;
        }

        if (i % 8 == 7) {
            j++;
        }
    }

    ret = csi_gpio_pin_config(sensor(dev)->handle, GPIO_MODE_PUSH_PULL, GPIO_DIRECTION_OUTPUT);
    if (ret < 0) {
        //LOGE(TAG, "set output err");
        //csi_irq_restore(lpsr);
        return -1;
    }

    ret = csi_gpio_pin_write(sensor(dev)->handle, 1);
    //csi_irq_restore(lpsr);

    return 0;
}

static int sensor_data_check(aos_dev_t *dev)
{
    int humi, humi2, temp, temp2, tol;

    /* get humi data*/
    humi = sensor(dev)->recv_buf[0];
    humi2 = sensor(dev)->recv_buf[1];
    /* get temp data*/
    temp = sensor(dev)->recv_buf[2];
    temp2 = sensor(dev)->recv_buf[3];
    /* get tol data*/
    tol = sensor(dev)->recv_buf[4];

    //return 0;
    if (tol == ((humi + humi2 + temp + temp2) & 0x0FF)) {
        return 0;
    } else if (humi > 10) {
        //LOGE(TAG, "get humi err");
        return -1;
    } else {
        //LOGE(TAG, "check err");
        return -1;
    }
}

static int yoc_sensor_dht22_fetch(aos_dev_t *dev)
{
    int ret/*, i*/;

    ret = sensor_read_data(dev);

    if (ret == 0) {
        ret = sensor_data_check(dev);
    }

    if (ret < 0) {
        return -1;
    }

    return 0;
}

static int yoc_sensor_dht22_get(aos_dev_t *dev, void *value, size_t size)
{
     if (size < sizeof (sensor_dht22_t)) {
        return -EINVAL;
    }

    ((sensor_dht22_t *)value)->rh = sensor(dev)->recv_buf[0] << 8 | sensor(dev)->recv_buf[1];

    if (((sensor(dev)->recv_buf[2]) & 0x80) == 0x80) {
        ((sensor_dht22_t *)value)->degree = -sensor(dev)->recv_buf[3];
    } else {
        ((sensor_dht22_t *)value)->degree = sensor(dev)->recv_buf[3];
    }

    return 0;
}

static sensor_driver_t sensor_driver = {
    .drv = {
        .name   = DHT22_DEV_NAME,
        .type   = "sensor",
        .init   = yoc_sensor_dht22_init,
        .uninit = yoc_sensor_dht22_uninit,
        .lpm    = yoc_sensor_dht22_lpm,
        .open   = yoc_sensor_dht22_open,
        .close  = yoc_sensor_dht22_close,
    },
    .fetch      = yoc_sensor_dht22_fetch,
    .getvalue   = yoc_sensor_dht22_get,
};

void sensor_dht22_register(sensor_pin_config_t *config, int idx)
{
    driver_register(&sensor_driver.drv, config, idx);
}
