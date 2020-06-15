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
#include <pin.h>
#include <aos/log.h>

#include <devices/iic.h>

#include <devices/hal/sensor_impl.h>

/*mpu6050*/
#define MPU6050_CONFIGURATION       0x1a /* set FSYNC and DLPF */
#define MPU6050_IDENTITY            0x75 /* verify the identity of the device */
#define MPU6050_PWR_REG1            0x6b /* power Register1       */
#define MPU6050_PWR_ENABLE          0x00 /* enable mpu6050 pwr reg value */
#define MPU6050_PWR_DISABLE         0x40 /* disable mpu6050 pwr reg value */
#define MPU6050_PWR_RESET           0x80 /* disable mpu6050 pwr reg value */

#define MPU6050_ACCEL_REG_BASE      0x3b /* acc Registers        */
#define MPU6050_ACCEL_REG_SIZE      6    /* acc Register number: X/Y/Z H & L  */

#define MPU6050_GYRO_REG_BASE       0x43 /* gyro Registers       */
#define MPU6050_GYRO_REG_SIZE       6    /* gyro Register number: X/Y/Z H & L */

#define MPU6050_TEMP_REG_BASE       0x41 /* temprature Registers       */
#define MPU6050_TEMP_REG_SIZE       2    /* temprature Register number: H & L */

#define MPU6050_INTSTATUS_REG_BASE  0x3a /* int status Registers       */

#define MPU6050_I2C_ADDR            0x68

static const char* TAG = "mpu6050";

static uint8_t mems_is_data_ready(aos_dev_t *dev);
static int mems_readreg(aos_dev_t *dev, uint8_t addr, uint8_t *data);
static int mems_writereg(aos_dev_t *dev, uint8_t addr, uint8_t data);
static int mems_readnreg(aos_dev_t *dev, uint8_t addr, uint8_t *data, int nbyte);

struct mpu6050_data_s {
    uint8_t xh;
    uint8_t xl;
    uint8_t yh;
    uint8_t yl;
    uint8_t zh;
    uint8_t zl;
};

struct mpu6050_temp_s {
    uint8_t temph;
    uint8_t templ;
};

typedef struct mpu6050_info_s {
    struct mpu6050_data_s acc;           /* acc data xh/xl/yh/yl/zh/zl */
    struct mpu6050_data_s gyro;          /* gyro data xh/xl/yh/yl/zh/zl */
    struct mpu6050_temp_s temp;          /* temprature data */
    uint8_t               addr;          /* i2c slave address */
} mpu6050_info_t;

typedef struct sensor_dev {
    aos_dev_t device;
    mpu6050_info_t info;
} sensor_dev_t;


static aos_dev_t *yoc_sensor_mpu6050_init(driver_t *drv, void *config, int id)
{
    sensor_dev_t *dev = (sensor_dev_t*)device_new(drv, sizeof(sensor_dev_t), id);

    iic_config_t *c = aos_malloc_check(sizeof(iic_config_t));

    c->mode         = MODE_MASTER;
    c->speed        = BUS_SPEED_STANDARD;
    c->addr_mode    = ADDR_7BIT;
    c->slave_addr   = MPU6050_I2C_ADDR;

    dev->device.config = c;

    return (aos_dev_t*)dev;
}

void yoc_sensor_mpu6050_uninit(aos_dev_t *dev)
{
    aos_free(dev->config);
    device_free(dev);
}

static int yoc_sensor_mpu6050_lpm(aos_dev_t *dev, int state)
{
    return 0;
}

static int yoc_sensor_mpu6050_open(aos_dev_t *dev)
{
    return 0;
}

static int yoc_sensor_mpu6050_close(aos_dev_t *dev)
{

    return 0;
}


static int yoc_sensor_mpu6050_fetch(aos_dev_t *dev)
{
    sensor_dev_t *sensor = (sensor_dev_t *)dev;
    iic_config_t *config = (iic_config_t *)dev->config;
    int ret;

    uint8_t data = 0;

    iic_config(dev, config);

    mems_readreg(dev,MPU6050_IDENTITY, &data);
    mems_writereg(dev, MPU6050_CONFIGURATION, 0x00);
    mems_writereg(dev, MPU6050_PWR_REG1, 0x00);


    if (!mems_is_data_ready(dev)) {
        LOGE(TAG, "iic busy");
        return -1;
    }

    /* get acc data x/y/z */
    ret = mems_readnreg(dev, MPU6050_ACCEL_REG_BASE, (uint8_t *)&sensor->info.acc, MPU6050_ACCEL_REG_SIZE);

    /* get gyro data x/y/z */
    ret = mems_readnreg(dev, MPU6050_GYRO_REG_BASE, (uint8_t *)&sensor->info.gyro, MPU6050_GYRO_REG_SIZE - 2);
    ret = mems_readnreg(dev, MPU6050_GYRO_REG_BASE + 4, (uint8_t *)&sensor->info.gyro.zh, 2);

    if (ret == -1) {
        LOGE(TAG, "I/O err");
        return -1;
    }

    return 0;
}

static int yoc_sensor_mpu6050_get(aos_dev_t *dev, void *value, size_t size)
{
    sensor_dev_t *sensor = (sensor_dev_t *)dev;

    if (size < sizeof (sensor_mpu6050_t)) {
        return -EINVAL;
    }

    /* calculate */
    ((sensor_mpu6050_t *)value)->acc_x = 61 * ((signed short)(sensor->info.acc.xh << 8) | sensor->info.acc.xl);
    ((sensor_mpu6050_t *)value)->acc_y = 61 * ((signed short)(sensor->info.acc.yh << 8) | sensor->info.acc.yl);
    ((sensor_mpu6050_t *)value)->acc_z = 61 * ((signed short)(sensor->info.acc.zh << 8) | sensor->info.acc.zl);

    ((sensor_mpu6050_t *)value)->gyro_x = 7633 * ((signed short)(sensor->info.gyro.xh << 8) | sensor->info.gyro.xl);
    ((sensor_mpu6050_t *)value)->gyro_y = 7633 * ((signed short)(sensor->info.gyro.yh << 8) | sensor->info.gyro.yl);
    ((sensor_mpu6050_t *)value)->gyro_z = 7633 * ((signed short)(sensor->info.gyro.zh << 8) | sensor->info.gyro.zl);

    return 0;
}

static uint8_t mems_is_data_ready(aos_dev_t *dev)
{
    uint8_t addr    = MPU6050_INTSTATUS_REG_BASE;
    iic_config_t *config = (iic_config_t *)dev->config;
    uint8_t status  = 0;
    /* read int status reg */

    int ret = iic_master_send(dev, config->slave_addr, &addr, 1, AOS_WAIT_FOREVER);

    if (ret < 0) {
        return 0;
    }

    ret = iic_master_recv(dev, config->slave_addr, &status, 1, AOS_WAIT_FOREVER);

    if (ret < 0) {
        return 0;
    }

    return status & 0x1;
}

static int mems_readreg(aos_dev_t *dev, uint8_t write_cmd, uint8_t *rxbuf)
{
    iic_config_t *config = (iic_config_t *)dev->config;

    int ret = iic_master_send(dev, config->slave_addr, &write_cmd, 1, AOS_WAIT_FOREVER);

    if (ret < 0) {
        return -1;
    }

    ret = iic_master_recv(dev, config->slave_addr, rxbuf, 1, AOS_WAIT_FOREVER);

    if (ret < 0) {
        return -1;
    }

    return 0;
}

static int mems_writereg(aos_dev_t *dev, uint8_t write_cmd, uint8_t txbuf)
{
    char buf[2] = {0};
    int  ret;
    iic_config_t *config = (iic_config_t *)dev->config;

    buf[0] = write_cmd;
    buf[1] = txbuf;

    ret = iic_master_send(dev, config->slave_addr, buf, 2, AOS_WAIT_FOREVER);

    if (ret == 2) {
        return 0;
    } else {
        return -1;
    }
}

static int mems_readnreg(aos_dev_t *dev, uint8_t write_cmd, uint8_t *rxbuf, int nbyte)
{
    iic_config_t *config = (iic_config_t *)dev->config;

    int ret = iic_master_send(dev, config->slave_addr, &write_cmd, 1, AOS_WAIT_FOREVER);

    if (ret < 0) {
        return -1;
    }

    ret = iic_master_recv(dev, config->slave_addr, rxbuf, nbyte, AOS_WAIT_FOREVER);

    if (ret < 0) {
        return -1;
    }

    return 0;
}

static sensor_driver_t sensor_driver = {
    .drv = {
        .name   = "mpu6050",
        .type   = "sensor",
        .init   = yoc_sensor_mpu6050_init,
        .uninit = device_free,
        .lpm    = yoc_sensor_mpu6050_lpm,
        .open   = yoc_sensor_mpu6050_open,
        .close  = yoc_sensor_mpu6050_close,
    },
    .fetch      = yoc_sensor_mpu6050_fetch,
    .getvalue   = yoc_sensor_mpu6050_get,
};

void sensor_mpu6050_register(void *config, int idx)
{
    driver_register(&sensor_driver.drv, config, idx);
}
