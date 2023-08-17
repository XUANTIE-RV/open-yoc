/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dev_internal.h>
#include <devices/impl/i2s_impl.h>

#define I2S_DRIVER(dev)  ((i2s_driver_t*)(dev->drv))
#define I2S_VAILD(dev) do { \
    if (device_valid(dev, "iis") != 0) \
        return -1; \
} while(0)

int rvm_hal_i2s_config(rvm_dev_t *dev, rvm_hal_i2s_config_t *config)
{
    int ret;

    I2S_VAILD(dev);

    device_lock(dev);
    ret = I2S_DRIVER(dev)->config(dev, config);
    device_unlock(dev);

    return ret;
}

int rvm_hal_i2s_config_get(rvm_dev_t *dev, rvm_hal_i2s_config_t *config)
{
    int ret;

    I2S_VAILD(dev);

    device_lock(dev);
    ret = I2S_DRIVER(dev)->config_get(dev, config);
    device_unlock(dev);

    return ret;
}

int rvm_hal_i2s_send(rvm_dev_t *dev, const void *data, size_t size, uint32_t timeout)
{
    int ret;

    I2S_VAILD(dev);

    device_lock(dev);
    ret = I2S_DRIVER(dev)->send(dev, data, size, timeout);
    device_unlock(dev);

    return ret;
}

int rvm_hal_i2s_recv(rvm_dev_t *dev, void *data, size_t size, uint32_t timeout)
{
    int ret;

    I2S_VAILD(dev);

    device_lock(dev);
    ret = I2S_DRIVER(dev)->recv(dev, data, size, timeout);
    device_unlock(dev);

    return ret;
}

int rvm_hal_i2s_pause(rvm_dev_t *dev)
{
    int ret;

    I2S_VAILD(dev);

    device_lock(dev);
    ret = I2S_DRIVER(dev)->pause(dev);
    device_unlock(dev);

    return ret;
}

int rvm_hal_i2s_resume(rvm_dev_t *dev)
{
    int ret;

    I2S_VAILD(dev);

    device_lock(dev);
    ret = I2S_DRIVER(dev)->resume(dev);
    device_unlock(dev);

    return ret;
}

int rvm_hal_i2s_stop(rvm_dev_t *dev)
{
    int ret;

    I2S_VAILD(dev);

    device_lock(dev);
    ret = I2S_DRIVER(dev)->stop(dev);
    device_unlock(dev);

    return ret;
}
