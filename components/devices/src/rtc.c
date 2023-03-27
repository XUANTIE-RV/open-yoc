/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dev_internal.h>
#include <devices/impl/rtc_impl.h>

#define RTC_DRIVER(dev)  ((rtc_driver_t*)(dev->drv))
#define RTC_VAILD(dev) do { \
    if (device_valid(dev, "rtc") != 0) \
        return -1; \
} while(0)

int rvm_hal_rtc_set_time(rvm_dev_t *dev, const struct tm *time)
{
    int ret;

    RTC_VAILD(dev);
    if (!time) {
        return -EINVAL;
    }
    device_lock(dev);
    ret = RTC_DRIVER(dev)->set_time(dev, time);
    device_unlock(dev);

    return ret;
}

int rvm_hal_rtc_get_time(rvm_dev_t *dev, struct tm *time)
{
    int ret;

    RTC_VAILD(dev);
    if (!time) {
        return -EINVAL;
    }

    device_lock(dev);
    ret = RTC_DRIVER(dev)->get_time(dev, time);
    device_unlock(dev);

    return ret;
}

uint32_t rvm_hal_rtc_get_alarm_remaining_time(rvm_dev_t *dev)
{
    int ret;

    if (device_valid(dev, "rtc")) {
        return 0;
    }

    device_lock(dev);
    ret = RTC_DRIVER(dev)->get_alarm_remaining_time(dev);
    device_unlock(dev);

    return ret;
}

int rvm_hal_rtc_set_alarm(rvm_dev_t *dev, const struct tm *time, rvm_hal_rtc_callback callback, void *arg)
{
    int ret;

    RTC_VAILD(dev);
    if (!time) {
        return -EINVAL;
    }
    device_lock(dev);
    ret = RTC_DRIVER(dev)->set_alarm(dev, time, callback, arg);
    device_unlock(dev);

    return ret;
}

int rvm_hal_rtc_cancel_alarm(rvm_dev_t *dev)
{
    int ret;

    RTC_VAILD(dev);

    device_lock(dev);
    ret = RTC_DRIVER(dev)->cancel_alarm(dev);
    device_unlock(dev);

    return ret;
}
