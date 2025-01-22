 /*
 * Copyright (C) 2017-2024 Alibaba Group Holding Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <aos/kernel.h>
#include <drv/wdt.h>
#include <drv/clk.h>
#include <devices/impl/wdt_impl.h>

#define TAG "wdt_drv"

typedef struct {
    rvm_dev_t      device;
    csi_wdt_t      handle;
    void           *arg;
    rvm_hal_wdt_callback callback;
} wdt_dev_t;

#define WDTDEV(dev) ((wdt_dev_t *)dev)

static rvm_dev_t *_wdt_init(driver_t *drv, void *config, int id)
{
    wdt_dev_t *rtc = (wdt_dev_t *)rvm_hal_device_new(drv, sizeof(wdt_dev_t), id);

    return (rvm_dev_t *)rtc;
}

#define _wdt_uninit rvm_hal_device_free

static int _wdt_open(rvm_dev_t *dev)
{
    csi_error_t ret = csi_wdt_init(&WDTDEV(dev)->handle, dev->id);

    if (ret != CSI_OK) {
        LOGE(TAG, "csi_wdt_init error");
        return -1;
    }
    return 0;
}

static int _wdt_close(rvm_dev_t *dev)
{
    csi_wdt_uninit(&WDTDEV(dev)->handle);
    return 0;
}

static int _wdt_lpm(rvm_dev_t *dev, int state)
{
#ifdef CONFIG_PM
    if (state == 0) {
        csi_wdt_disable_pm(&WDTDEV(dev)->handle);
    } else {
        csi_wdt_enable_pm(&WDTDEV(dev)->handle);
    }
#endif
    return 0;
}

static int _wdt_clock(rvm_dev_t *dev, bool enable)
{
    if (enable) {
        csi_clk_enable(&WDTDEV(dev)->handle.dev);
    } else {
        csi_clk_disable(&WDTDEV(dev)->handle.dev);
    }
    return 0;
}

int _wdt_set_timeout(rvm_dev_t *dev, uint32_t ms)
{
    csi_error_t ret = csi_wdt_set_timeout(&WDTDEV(dev)->handle, ms);

    if (ret != CSI_OK) {
        LOGE(TAG, "csi_wdt_set_timeout error");
        return -1;
    }
    return 0;
}

int _wdt_start(rvm_dev_t *dev)
{
    csi_error_t ret = csi_wdt_start(&WDTDEV(dev)->handle);

    if (ret != CSI_OK) {
        LOGE(TAG, "csi_wdt_start error");
        return -1;
    }
    return 0;
}

int _wdt_stop(rvm_dev_t *dev)
{
    csi_wdt_stop(&WDTDEV(dev)->handle);
    return 0;
}

int _wdt_feed(rvm_dev_t *dev)
{
    csi_error_t ret = csi_wdt_feed(&WDTDEV(dev)->handle);

    if (ret != CSI_OK) {
        LOGE(TAG, "csi_wdt_feed error");
        return -1;
    }
    return 0;
}

uint32_t _wdt_get_remaining_time(rvm_dev_t *dev)
{
    return csi_wdt_get_remaining_time(&WDTDEV(dev)->handle);
}

static void _wdt_callback(csi_wdt_t *wdt, void *arg)
{
    rvm_dev_t *dev = (rvm_dev_t *)arg;
    if (WDTDEV(dev)->callback) {
        WDTDEV(dev)->callback(dev, WDTDEV(dev)->arg);
    }
}

int _wdt_attach_callback(rvm_dev_t *dev, rvm_hal_wdt_callback callback, void *arg)
{
    WDTDEV(dev)->arg = arg;
    WDTDEV(dev)->callback = callback;
    csi_error_t ret = csi_wdt_attach_callback(&WDTDEV(dev)->handle, _wdt_callback, dev);

    if (ret != CSI_OK) {
        LOGE(TAG, "csi_wdt_attach_callback error");
        return -1;
    }
    return 0;
}

int _wdt_detach_callback(rvm_dev_t *dev)
{
    csi_wdt_detach_callback(&WDTDEV(dev)->handle);
    return 0;
}

static wdt_driver_t wdt_driver = {
    .drv = {
        .name   = "wdt",
        .init   = _wdt_init,
        .uninit = _wdt_uninit,
        .open   = _wdt_open,
        .close  = _wdt_close,
        .lpm    = _wdt_lpm,
        .clk_en = _wdt_clock
    },
    .set_timeout        = _wdt_set_timeout,
    .start              = _wdt_start,
    .stop               = _wdt_stop,
    .feed               = _wdt_feed,
    .get_remaining_time = _wdt_get_remaining_time,
    .attach_callback    = _wdt_attach_callback,
    .detach_callback    = _wdt_detach_callback
};

void rvm_wdt_drv_register(int idx)
{
    rvm_driver_register(&wdt_driver.drv, NULL, idx);
}
