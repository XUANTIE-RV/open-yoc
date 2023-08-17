/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dev_internal.h>
#include <devices/impl/input_impl.h>

#define INPUT_DRIVER(dev)  ((input_driver_t*)(dev->drv))
#define INPUT_VAILD(dev) do { \
    if (device_valid(dev, "input") != 0) \
        return -1; \
} while(0)

int rvm_hal_input_set_event(rvm_dev_t *dev, rvm_hal_input_event event_cb, void *priv)
{
    int ret;

    INPUT_VAILD(dev);

    device_lock(dev);
    ret = INPUT_DRIVER(dev)->set_event(dev, event_cb, priv);
    device_unlock(dev);

    return ret;
}

int rvm_hal_input_unset_event(rvm_dev_t *dev, rvm_hal_input_event event_cb)
{
    int ret;

    INPUT_VAILD(dev);

    device_lock(dev);
    ret = INPUT_DRIVER(dev)->unset_event(dev, event_cb);
    device_unlock(dev);

    return ret;
}

int rvm_hal_input_read(rvm_dev_t *dev, void *data, uint32_t size, unsigned int timeout_ms)
{
    int ret;

    INPUT_VAILD(dev);

    device_lock(dev);
    ret = INPUT_DRIVER(dev)->read(dev, data, size, timeout_ms);
    device_unlock(dev);

    return ret;
}
