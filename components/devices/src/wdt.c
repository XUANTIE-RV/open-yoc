/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dev_internal.h>
#include <devices/impl/wdt_impl.h>

#define WDT_DRIVER(dev)  ((wdt_driver_t*)(dev->drv))
#define WDT_VAILD(dev) do { \
    if (device_valid(dev, "wdt") != 0) \
        return -1; \
} while(0)

int rvm_hal_wdt_set_timeout(rvm_dev_t *dev, uint32_t ms)
{
    int ret;

    WDT_VAILD(dev);

    device_lock(dev);
    ret = WDT_DRIVER(dev)->set_timeout(dev, ms);
    device_unlock(dev);

    return ret;
}

int rvm_hal_wdt_start(rvm_dev_t *dev)
{
    int ret;

    WDT_VAILD(dev);

    device_lock(dev);
    ret = WDT_DRIVER(dev)->start(dev);
    device_unlock(dev);

    return ret;
}

int rvm_hal_wdt_stop(rvm_dev_t *dev)
{
    int ret;

    WDT_VAILD(dev);

    device_lock(dev);
    ret = WDT_DRIVER(dev)->stop(dev);
    device_unlock(dev);

    return ret;
}

int rvm_hal_wdt_feed(rvm_dev_t *dev)
{
    int ret;

    WDT_VAILD(dev);

    device_lock(dev);
    ret = WDT_DRIVER(dev)->feed(dev);
    device_unlock(dev);

    return ret;
}

uint32_t rvm_hal_wdt_get_remaining_time(rvm_dev_t *dev)
{
    uint32_t ret;

    if (device_valid(dev, "wdt") != 0)
        return 0;
    device_lock(dev);
    ret = WDT_DRIVER(dev)->get_remaining_time(dev);
    device_unlock(dev);

    return ret;
}

int rvm_hal_wdt_attach_callback(rvm_dev_t *dev, rvm_hal_wdt_callback callback, void *arg)
{
    int ret;

    WDT_VAILD(dev);

    device_lock(dev);
    ret = WDT_DRIVER(dev)->attach_callback(dev, callback, arg);
    device_unlock(dev);

    return ret;
}

int rvm_hal_wdt_detach_callback(rvm_dev_t *dev)
{
    int ret;

    WDT_VAILD(dev);

    device_lock(dev);
    ret = WDT_DRIVER(dev)->detach_callback(dev);
    device_unlock(dev);

    return ret;
}
