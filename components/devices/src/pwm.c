/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dev_internal.h>
#include <devices/impl/pwm_impl.h>

#define PWM_DRIVER(dev)  ((pwm_driver_t*)(dev->drv))
#define PWM_VAILD(dev) do { \
    if (device_valid(dev, "pwm") != 0) \
        return -1; \
} while(0)

int rvm_hal_pwm_config(rvm_dev_t *dev, rvm_hal_pwm_config_t *config, uint8_t channel)
{
    int ret;

    PWM_VAILD(dev);

    device_lock(dev);
    ret = PWM_DRIVER(dev)->config(dev, config, channel);
    device_unlock(dev);

    return ret;
}

int rvm_hal_pwm_config_get(rvm_dev_t *dev, rvm_hal_pwm_config_t *config, uint8_t channel)
{
    int ret;

    PWM_VAILD(dev);

    device_lock(dev);
    ret = PWM_DRIVER(dev)->config_get(dev, config, channel);
    device_unlock(dev);

    return ret;
}

int rvm_hal_pwm_start(rvm_dev_t *dev, uint8_t channel)
{
    int ret;

    PWM_VAILD(dev);

    device_lock(dev);
    ret = PWM_DRIVER(dev)->start(dev, channel);
    device_unlock(dev);

    return ret;
}

int rvm_hal_pwm_stop(rvm_dev_t *dev, uint8_t channel)
{
    int ret;

    PWM_VAILD(dev);

    device_lock(dev);
    ret = PWM_DRIVER(dev)->stop(dev, channel);
    device_unlock(dev);

    return ret;
}
