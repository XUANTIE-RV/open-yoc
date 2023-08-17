/*
 * Copyright (C) 2023 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dev_internal.h>
#include <devices/impl/timer_impl.h>

#define TIMER_DRIVER(dev)  ((timer_driver_t*)(dev->drv))
#define TIMER_VAILD(dev) do { \
    if (device_valid(dev, "timer") != 0) \
        return -1; \
} while(0)

int rvm_hal_timer_start(rvm_dev_t *dev, uint32_t timeout_us)
{
    int ret;

    TIMER_VAILD(dev);

    device_lock(dev);
    ret = TIMER_DRIVER(dev)->start(dev, timeout_us);
    device_unlock(dev);

    return ret;
}

int rvm_hal_timer_oneshot_start(rvm_dev_t *dev, uint32_t timeout_us)
{
    int ret;

    TIMER_VAILD(dev);

    device_lock(dev);
    ret = TIMER_DRIVER(dev)->oneshot_start(dev, timeout_us);
    device_unlock(dev);

    return ret;
}

int rvm_hal_timer_stop(rvm_dev_t *dev)
{
    int ret;

    TIMER_VAILD(dev);

    device_lock(dev);
    ret = TIMER_DRIVER(dev)->stop(dev);
    device_unlock(dev);

    return ret;    
}

int rvm_hal_timer_attach_callback(rvm_dev_t *dev, rvm_hal_timer_callback callback, void *arg)
{
    int ret;

    TIMER_VAILD(dev);

    device_lock(dev);
    ret = TIMER_DRIVER(dev)->attach_callback(dev, callback, arg);
    device_unlock(dev);

    return ret;
}

int rvm_hal_timer_detach_callback(rvm_dev_t *dev)
{
    int ret;

    TIMER_VAILD(dev);

    device_lock(dev);
    ret = TIMER_DRIVER(dev)->detach_callback(dev);
    device_unlock(dev);

    return ret;
}

uint32_t rvm_hal_timer_get_remaining_value(rvm_dev_t *dev)
{
    int ret;

    TIMER_VAILD(dev);

    device_lock(dev);
    ret = TIMER_DRIVER(dev)->get_remaining_value(dev);
    device_unlock(dev);

    return ret;
}

uint32_t rvm_hal_timer_get_load_value(rvm_dev_t *dev)
{
    int ret;

    TIMER_VAILD(dev);

    device_lock(dev);
    ret = TIMER_DRIVER(dev)->get_load_value(dev);
    device_unlock(dev);

    return ret;
}
