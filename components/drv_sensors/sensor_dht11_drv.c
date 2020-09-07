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
#define TAG "dht11"

#define SENSOR_DATA_LEN         40
#define DHT_SENSOR_MDELAY       30 /* ms*/
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

static int yoc_sensor_get_level(aos_dev_t *dev)
{
    bool value;

    csi_gpio_pin_read(sensor(dev)->handle, &value);

    return value;
}

static inline int yoc_sensor_read_level(aos_dev_t *dev, int count, int sign_value)
{
    bool value;

    do {
        csi_gpio_pin_read(sensor(dev)->handle, &value);

        if (count-- == 0) {
            return -1;
        }
    } while (value == sign_value);

    return 0;
}


static aos_dev_t *yoc_sensor_dht11_init(driver_t *drv, void *config, int id)
{
    sensor_dev_t *dev = (sensor_dev_t*)device_new(drv, sizeof(sensor_dev_t), id);

    dev->device.config = config;

    return (aos_dev_t*)dev;
}

#define yoc_sensor_dht11_uninit device_free

static int yoc_sensor_dht11_lpm(aos_dev_t *dev, int state)
{
    return 0;
}

static int yoc_sensor_dht11_open(aos_dev_t *dev)
{
    sensor(dev)->handle = csi_gpio_pin_initialize(((sensor_pin_config_t *)sensor(dev)->device.config)->pin, NULL);
    csi_gpio_pin_config(sensor(dev)->handle, GPIO_MODE_OPEN_DRAIN, GPIO_DIRECTION_OUTPUT);
    csi_gpio_pin_write(sensor(dev)->handle, 1);

    return 0;
}

static int yoc_sensor_dht11_close(aos_dev_t *dev)
{
    csi_gpio_pin_uninitialize(sensor(dev)->handle);
    return 0;
}

static int yoc_sensor_read_data(aos_dev_t *dev)
{
    int i = 0;
    int j = 0;

    // wait DHT11 device state machine reset after one transfer over
    sleep(2);

    memset(sensor(dev)->recv_buf, 0, sizeof(sensor(dev)->recv_buf));

    csi_gpio_pin_config(sensor(dev)->handle, GPIO_MODE_OPEN_DRAIN, GPIO_DIRECTION_OUTPUT);
    /* set gpio low level*/
    csi_gpio_pin_write(sensor(dev)->handle, 0);
    udelay(20000);
    /* set gpio high level*/
    csi_gpio_pin_write(sensor(dev)->handle, 1);
    udelay(40);

    /* set gpio mode: input*/
    csi_gpio_pin_config(sensor(dev)->handle, GPIO_MODE_PULLUP, GPIO_DIRECTION_INPUT);

    /* wait high level*/
    if (yoc_sensor_read_level(dev, DHT_SENSOR_COUNTS, 0) < 0) {
        LOGE(TAG, "wait high err");
        return -1;
    }

    /* start read bit*/
    for (i = 0; i < SENSOR_DATA_LEN; i++) {
        // wait low level
        if (yoc_sensor_read_level(dev, DHT_SENSOR_COUNTS, 1) < 0) {
            LOGE(TAG, "wait data low err, %d\n", i);
            return -1;
        }
        // wait high levels
        if (yoc_sensor_read_level(dev, DHT_SENSOR_COUNTS, 0) < 0) {
            LOGE(TAG, "wait data high err, %d\n", i);
            return -1;
        }

        udelay(50);

        if (yoc_sensor_get_level(dev) == 0) {
            sensor(dev)->recv_buf[j] = sensor(dev)->recv_buf[j] << 1;
        } else {
            sensor(dev)->recv_buf[j] = (sensor(dev)->recv_buf[j] << 1) | 1;
        }

        if (i % 8 == 7) {
            j++;
        }
    }

    csi_gpio_pin_config(sensor(dev)->handle, GPIO_MODE_OPEN_DRAIN, GPIO_DIRECTION_OUTPUT);
    csi_gpio_pin_write(sensor(dev)->handle, 1);

    return 0;
}

static int yoc_sensor_data_check(aos_dev_t *dev)
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

    if (tol == (humi + humi2 + temp + temp2)) {
        return 0;
    } else if (humi > 10) {
        LOGE(TAG, "get humi err\n");
        return -1;
    } else {
        LOGE(TAG, "check err");
        return -1;
    }
}

static int yoc_sensor_dht11_fetch(aos_dev_t *dev)
{
    int ret;

    ret = yoc_sensor_read_data(dev);
    if (ret == 0) {
        ret = yoc_sensor_data_check(dev);
    }

    if (ret < 0) {
        return -1;
    }

    return 0;
}

static int yoc_sensor_dht11_get(aos_dev_t *dev, void *value, size_t size)
{
    sensor_dev_t *sensor = (sensor_dev_t*)dev;

    if ((size < sizeof (sensor_dht11_t)) || (value == NULL)) {
        return -EINVAL;
    }

    ((sensor_dht11_t *)value)->rh = sensor->recv_buf[0] * 10 + sensor->recv_buf[1];
    ((sensor_dht11_t *)value)->degree = sensor->recv_buf[2] * 10 + sensor->recv_buf[3];

    return 0;
}

static sensor_driver_t sensor_driver = {
    .drv = {
        .name   = DHT11_DEV_NAME,
        .type   = "sensor",
        .init   = yoc_sensor_dht11_init,
        .uninit = yoc_sensor_dht11_uninit,
        .lpm    = yoc_sensor_dht11_lpm,
        .open   = yoc_sensor_dht11_open,
        .close  = yoc_sensor_dht11_close,
    },
    .fetch      = yoc_sensor_dht11_fetch,
    .getvalue   = yoc_sensor_dht11_get,
};

void sensor_dht11_register(sensor_pin_config_t *config, int idx)
{
    driver_register(&sensor_driver.drv, (void *)config, idx);
}
