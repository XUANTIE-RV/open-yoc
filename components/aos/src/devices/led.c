/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <devices/led.h>

#define LED_DRIVER(dev)  ((led_driver_t*)(dev->drv))
#define LED_VAILD(dev) do { \
    if (device_valid(dev, "led") != 0) \
        return -1; \
} while(0)

int led_control(aos_dev_t *dev, int color, int on_time, int off_time)
{
    int ret;

    LED_VAILD(dev);

    device_lock(dev);
    ret = LED_DRIVER(dev)->control(dev, color, on_time, off_time);
    device_unlock(dev);

    return ret < 0 ? -EIO : 0;
}
