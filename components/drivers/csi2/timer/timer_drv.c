/*
 * Copyright (C) 2023 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <aos/kernel.h>
#include <drv/timer.h>
#include <drv/clk.h>
#include <devices/impl/timer_impl.h>

#define TAG "timer_drv"

typedef struct {
    rvm_dev_t               device;
    csi_timer_t             handle;
    rvm_hal_timer_callback  callback;
    void                    *arg;
    uint8_t                 oneshot;
    uint8_t                 stopped;
} timer_dev_t;

#define TIMERDEV(dev) ((timer_dev_t *)dev)

static rvm_dev_t *_timer_init(driver_t *drv, void *config, int id)
{
    timer_dev_t *timer = (timer_dev_t *)rvm_hal_device_new(drv, sizeof(timer_dev_t), id);

    return (rvm_dev_t *)timer;
}

#define _timer_uninit rvm_hal_device_free

static int _timer_open(rvm_dev_t *dev)
{
    csi_error_t ret = csi_timer_init(&TIMERDEV(dev)->handle, dev->id);

    if (ret != CSI_OK) {
        LOGE(TAG, "csi_timer_init error");
        return -1;
    }
    return 0;    
}

static int _timer_close(rvm_dev_t *dev)
{
    csi_timer_uninit(&TIMERDEV(dev)->handle);
    return 0;
}

static int _timer_lpm(rvm_dev_t *dev, int state)
{
#ifdef CONFIG_PM
    if (state == 0) {
        csi_timer_disable_pm(&TIMERDEV(dev)->handle);
    } else {
        csi_timer_enable_pm(&TIMERDEV(dev)->handle);
    }
#endif
    return 0;
}

static int _timer_clock(rvm_dev_t *dev, bool enable)
{
    if (enable) {
        csi_clk_enable(&TIMERDEV(dev)->handle.dev);
    } else {
        csi_clk_disable(&TIMERDEV(dev)->handle.dev);
    }
    return 0;
}

static int _timer_start(rvm_dev_t *dev, uint32_t timeout_us)
{
    csi_error_t ret= csi_timer_start(&TIMERDEV(dev)->handle, timeout_us);

    if (ret != CSI_OK) {
        LOGE(TAG, "csi_timer_start error");
        return -1;
    }
    TIMERDEV(dev)->oneshot = 0;
    TIMERDEV(dev)->stopped = 0;
    return 0;
}

static int _timer_oneshot_start(rvm_dev_t *dev, uint32_t timeout_us)
{
    csi_error_t ret= csi_timer_start(&TIMERDEV(dev)->handle, timeout_us);

    if (ret != CSI_OK) {
        LOGE(TAG, "csi_timer_start error");
        return -1;
    }
    TIMERDEV(dev)->oneshot = 1;
    TIMERDEV(dev)->stopped = 0;
    return 0; 
}

static int _timer_stop(rvm_dev_t *dev)
{
    if (TIMERDEV(dev)->stopped == 1)
        return 0;
    csi_timer_stop(&TIMERDEV(dev)->handle);
    TIMERDEV(dev)->stopped = 1;
    return 0;
}

static void _timer_callback(csi_timer_t *timer, void *arg)
{
    rvm_dev_t *dev = (rvm_dev_t *)arg;
    if (TIMERDEV(dev)->callback) {
        TIMERDEV(dev)->callback(dev, TIMERDEV(dev)->arg);
        if (TIMERDEV(dev)->oneshot == 1) {
            if (TIMERDEV(dev)->stopped == 0) {
                csi_timer_stop(&TIMERDEV(dev)->handle);
                TIMERDEV(dev)->stopped = 1;
            }
        }
    }
}

static int _timer_attach_callback(rvm_dev_t *dev, rvm_hal_timer_callback callback, void *arg)
{
    TIMERDEV(dev)->callback = callback;
    TIMERDEV(dev)->arg = arg;
    csi_error_t ret = csi_timer_attach_callback(&TIMERDEV(dev)->handle, _timer_callback, dev);

    if (ret != CSI_OK) {
        LOGE(TAG, "csi_timer_attach_callback error");
        return -1;
    }
    return 0;
}

static int _timer_detach_callback(rvm_dev_t *dev)
{
    csi_timer_detach_callback(&TIMERDEV(dev)->handle);
    return 0;  
}

static uint32_t _timer_get_remaining_value(rvm_dev_t *dev)
{
    return csi_timer_get_remaining_value(&TIMERDEV(dev)->handle);
}

static uint32_t _timer_get_load_value(rvm_dev_t *dev)
{
    return csi_timer_get_load_value(&TIMERDEV(dev)->handle);
}

static timer_driver_t timer_driver = {
    .drv = {
        .name   = "timer",
        .init   = _timer_init,
        .uninit = _timer_uninit,
        .open   = _timer_open,
        .close  = _timer_close,
        .lpm    = _timer_lpm,
        .clk_en = _timer_clock
    },
    .start                  = _timer_start,
    .oneshot_start          = _timer_oneshot_start,
    .stop                   = _timer_stop,
    .attach_callback        = _timer_attach_callback,
    .detach_callback        = _timer_detach_callback,
    .get_remaining_value    = _timer_get_remaining_value,
    .get_load_value         = _timer_get_load_value,
};

void rvm_timer_drv_register(int idx)
{
    rvm_driver_register(&timer_driver.drv, NULL, idx);
}
