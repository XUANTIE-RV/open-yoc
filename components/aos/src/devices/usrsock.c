/*
 * Copyright (C) 2020 FishSemi Inc. All rights reserved.
 */
/******************************************************************************
 * @file     usrsock.c
 * @brief    user sock device
 * @version  V1.0
 * @date     7. Feb 2020
 ******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <devices/hal/usrsock_impl.h>

#define USRSOCK_DRIVER(dev)  ((usrsock_driver_t*)(dev->drv))
#define USRSOCK_VAILD(dev) do { \
    if (device_valid(dev, "usrsock") != 0) \
        return -1; \
} while(0)

ssize_t usrsock_write(aos_dev_t *dev, const void *data, size_t size)
{
    ssize_t ret;

    if (size == 0 || NULL == data) {
        return -EINVAL;
    }

    USRSOCK_VAILD(dev);

    device_lock(dev);
    ret = USRSOCK_DRIVER(dev)->write(dev, data, size);
    device_unlock(dev);

    return ret;
}

ssize_t usrsock_read(aos_dev_t *dev, void *data, size_t size)
{
    ssize_t ret;

    USRSOCK_VAILD(dev);

    device_lock(dev);
    ret = USRSOCK_DRIVER(dev)->read(dev, data, size);
    device_unlock(dev);

    return ret;
}

void usrsock_set_event(aos_dev_t *dev, void (*event)(aos_dev_t *dev, int event_id, void *priv), void *priv)
{
    aos_check_param(dev);
    if (device_valid(dev, "usrsock") != 0)
        return;

    device_lock(dev);
    USRSOCK_DRIVER(dev)->set_event(dev, event, priv);
    device_unlock(dev);
}

