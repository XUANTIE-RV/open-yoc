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
#include <unistd.h>
#include <errno.h>
#include <drv/iic.h>
#include <pin.h>
#include <aos/log.h>
#include <aos/kernel.h>

#include <devices/iic.h>

#include <devices/hal/sensor_impl.h>

/* sht20*/
#define SHT20_TEMP_REG_BASE     0xf3 /* temprature Registers */
#define SHT20_TEMP_REG_SIZE     3    /* temprature Register number: H & L */

#define SHT20_HUMI_REG_BASE     0xf5 /* humidity Registers */
#define SHT20_HUMI_REG_SIZE     3    /* humidity Register number */

#define SHT20_PWR_RESET         0xFE /* disable SHT20 pwr reg value */

#define SHT20_I2C_ADDR          0x40

static const char* TAG = "sht20";

typedef struct sensor_dev {
    aos_dev_t device;
    char recv_temp_buf[2];
    char recv_humi_buf[2];
    char tempaddr;
    char humiaddr;
    int  flag;

} sensor_dev_t;

static aos_dev_t *yoc_sensor_sht20_init(driver_t *drv,void *config, int id)
{
    sensor_dev_t *dev = (sensor_dev_t*)device_new(drv, sizeof(sensor_dev_t), id);

    iic_config_t *c = aos_malloc_check(sizeof(iic_config_t));

    c->mode         = MODE_MASTER;
    c->speed        = BUS_SPEED_STANDARD;
    c->addr_mode    = ADDR_7BIT;
    c->slave_addr   = SHT20_I2C_ADDR;

    dev->device.config = c;

    return (aos_dev_t*)dev;
}

void yoc_sensor_sht20_uninit(aos_dev_t *dev)
{
    aos_free(dev->config);
    device_free(dev);
}

static int yoc_sensor_sht20_lpm(aos_dev_t *dev, int state)
{
    return 0;
}

static int yoc_sensor_sht20_open(aos_dev_t *dev)
{
    sensor_dev_t *sensor = (sensor_dev_t *)dev;

    sensor->tempaddr = SHT20_TEMP_REG_BASE;
    sensor->humiaddr = SHT20_HUMI_REG_BASE;

    return 0;
}

static int yoc_sensor_sht20_close(aos_dev_t *dev)
{
    sensor_dev_t *sensor = (sensor_dev_t *)dev;

    sensor->tempaddr = 0;
    sensor->humiaddr = 0;

    return 0;
}

static int yoc_sensor_sht20_fetch(aos_dev_t *dev)
{
    sensor_dev_t *sensor = (sensor_dev_t *)dev;
    iic_config_t *config = (iic_config_t *)dev->config;
    int ret = -1;

    iic_config(dev, config);

    ret = iic_master_send(dev, config->slave_addr, &sensor->humiaddr, 1, AOS_WAIT_FOREVER);

    if (ret < 0) {
        LOGE(TAG, "send err");
        return -1;
    }

    aos_msleep(80);
    ret = iic_master_recv(dev, config->slave_addr, &sensor->recv_humi_buf, 2, AOS_WAIT_FOREVER);

    if (ret < 0) {
        LOGE(TAG, "recv err");
        return -1;
    }

    ret = iic_master_send(dev, config->slave_addr, &sensor->tempaddr, 1, AOS_WAIT_FOREVER);

    if (ret < 0) {
        LOGE(TAG, "send err");
        return -1;
    }

    aos_msleep(80);
    ret = iic_master_recv(dev, config->slave_addr, &sensor->recv_temp_buf, 2, AOS_WAIT_FOREVER);

    if (ret < 0) {
        LOGE(TAG, "recv err");
        return -1;
    }

    return 0;
}
static int yoc_sensor_sht20_get(aos_dev_t *dev, void *value, size_t size)
{
    sensor_dev_t *sensor = (sensor_dev_t *)dev;

    if (size < sizeof (sensor_sht20_t)) {
        return -EINVAL;
    }

    uint32_t val;

    val = ((unsigned short)sensor->recv_humi_buf[0] << 8 | sensor->recv_humi_buf[1]);
    val &= ~0x0003;

    ((sensor_sht20_t *)value)->rh = ((val * 1907) - 6000000) / 100000;

    if (((sensor_sht20_t *)value)->rh > 1000) {
        ((sensor_sht20_t *)value)->rh = 1000;
    }

    val = sensor->recv_temp_buf[0];
    val <<= 8;
    val |= sensor->recv_temp_buf[1];
    val &= ~0x3u;

    ((sensor_sht20_t *)value)->degree = ((val * 2681) - 46850000) / 100000;

    return 0;
}

static sensor_driver_t sensor_driver = {
    .drv = {
        .name   = "sht20",
        .type   = "sensor",
        .init   = yoc_sensor_sht20_init,
        .uninit = yoc_sensor_sht20_uninit,
        .lpm    = yoc_sensor_sht20_lpm,
        .open   = yoc_sensor_sht20_open,
        .close  = yoc_sensor_sht20_close,
    },
    .fetch      = yoc_sensor_sht20_fetch,
    .getvalue   = yoc_sensor_sht20_get,
};

void sensor_sht20_register(void *config, int idx)
{
    driver_register(&sensor_driver.drv, config, idx);
}
