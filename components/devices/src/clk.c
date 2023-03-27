/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <dev_internal.h>
#include <devices/impl/clk_impl.h>

#define CLK_DRIVER(dev)  ((clk_driver_t*)(dev->drv))
#define CLK_VAILD(dev) do { \
    if (device_valid(dev, "clk") != 0) \
        return -1; \
} while(0)

int rvm_hal_clk_enable(rvm_dev_t *dev, const char *dev_name)
{
    int ret;

    CLK_VAILD(dev);
    if (!dev_name) {
        return -EINVAL;
    }

    device_lock(dev);
    ret = CLK_DRIVER(dev)->enable(dev, dev_name);
    device_unlock(dev);

    return ret;
}

int rvm_hal_clk_disable(rvm_dev_t *dev, const char *dev_name)
{
    int ret;

    CLK_VAILD(dev);
    if (!dev_name) {
        return -EINVAL;
    }

    device_lock(dev);
    ret = CLK_DRIVER(dev)->disable(dev, dev_name);
    device_unlock(dev);

    return ret;
}

int rvm_hal_clk_get_freq(rvm_dev_t *dev, rvm_hal_clk_id_t clk_id, uint32_t idx, uint32_t *freq)
{
    int ret;

    CLK_VAILD(dev);
    if (clk_id > RVM_HAL_END_CLK_ID - 1 || !freq) {
        return -EINVAL;
    }

    device_lock(dev);
    ret = CLK_DRIVER(dev)->get_freq(dev, clk_id, idx, freq);
    device_unlock(dev);

    return ret;
}

int rvm_hal_clk_set_freq(rvm_dev_t *dev, rvm_hal_clk_id_t clk_id, uint32_t idx, uint32_t freq)
{
    int ret;

    CLK_VAILD(dev);
    if (clk_id > RVM_HAL_END_CLK_ID - 1) {
        return -EINVAL;
    }

    device_lock(dev);
    ret = CLK_DRIVER(dev)->set_freq(dev, clk_id, idx, freq);
    device_unlock(dev);

    return ret;
}
