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
#include <drv/rtc.h>
#include <drv/clk.h>
#include <devices/impl/rtc_impl.h>

#define TAG "rtc_drv"

typedef struct {
    rvm_dev_t      device;
    csi_rtc_t      handle;
    void           *arg;
    rvm_hal_rtc_callback callback;
} rtc_dev_t;

#define RTCDEV(dev) ((rtc_dev_t *)dev)

static rvm_dev_t *_rtc_init(driver_t *drv, void *config, int id)
{
    rtc_dev_t *rtc = (rtc_dev_t *)rvm_hal_device_new(drv, sizeof(rtc_dev_t), id);

    return (rvm_dev_t *)rtc;
}

#define _rtc_uninit rvm_hal_device_free

static int _rtc_open(rvm_dev_t *dev)
{
    csi_error_t ret = csi_rtc_init(&RTCDEV(dev)->handle, dev->id);

    if (ret != CSI_OK) {
        LOGE(TAG, "csi_rtc_init error");
        return -1;
    }
    return 0;
}

static int _rtc_close(rvm_dev_t *dev)
{
    csi_rtc_uninit(&RTCDEV(dev)->handle);
    return 0;
}

static int _rtc_lpm(rvm_dev_t *dev, int state)
{
#ifdef CONFIG_PM
    if (state == 0) {
        csi_rtc_disable_pm(&RTCDEV(dev)->handle);
    } else {
        csi_rtc_enable_pm(&RTCDEV(dev)->handle);
    }
#endif
    return 0;
}

static int _rtc_clock(rvm_dev_t *dev, bool enable)
{
    if (enable) {
        csi_clk_enable(&RTCDEV(dev)->handle.dev);
    } else {
        csi_clk_disable(&RTCDEV(dev)->handle.dev);
    }
    return 0;
}

static int _rtc_set_time(rvm_dev_t *dev, const struct tm *time)
{
    csi_rtc_time_t rtc_time;

    rtc_time.tm_year = time->tm_year;
    rtc_time.tm_mon  = time->tm_mon;
    rtc_time.tm_mday = time->tm_mday;
    rtc_time.tm_hour = time->tm_hour;
    rtc_time.tm_min  = time->tm_min;
    rtc_time.tm_sec  = time->tm_sec;
    rtc_time.tm_wday = time->tm_wday;
    rtc_time.tm_yday = time->tm_yday;

    csi_error_t ret =  csi_rtc_set_time(&RTCDEV(dev)->handle, (const csi_rtc_time_t *)&rtc_time);
    if (ret != CSI_OK) {
        LOGE(TAG, "csi_rtc_set_time error");
        return -1;
    }
    return 0;
}

int _rtc_get_time(rvm_dev_t *dev, struct tm *time)
{
    csi_rtc_time_t last_time;

    csi_error_t ret = csi_rtc_get_time(&RTCDEV(dev)->handle, &last_time);

    if (ret != CSI_OK) {
        LOGE(TAG, "csi_rtc_get_time error");
        return -1;
    }

    time->tm_year = last_time.tm_year;
    time->tm_mon  = last_time.tm_mon;
    time->tm_mday = last_time.tm_mday;
    time->tm_hour = last_time.tm_hour;
    time->tm_min  = last_time.tm_min;
    time->tm_sec  = last_time.tm_sec;
    time->tm_wday = last_time.tm_wday;
    time->tm_yday = last_time.tm_yday;
    time->tm_isdst = 0;
    return 0;
}

uint32_t _rtc_get_alarm_remaining_time(rvm_dev_t *dev)
{
    return csi_rtc_get_alarm_remaining_time(&RTCDEV(dev)->handle);
}

static void _rtc_callback(csi_rtc_t *rtc, void *arg)
{
    rvm_dev_t *dev = (rvm_dev_t *)arg;

    if (RTCDEV(dev)->callback) {
        RTCDEV(dev)->callback(dev, RTCDEV(dev)->arg);
    }
}

int _rtc_set_alarm(rvm_dev_t *dev, const struct tm *time, rvm_hal_rtc_callback callback, void *arg)
{
    csi_rtc_time_t rtc_time;

    rtc_time.tm_year = time->tm_year;
    rtc_time.tm_mon  = time->tm_mon;
    rtc_time.tm_mday = time->tm_mday;
    rtc_time.tm_hour = time->tm_hour;
    rtc_time.tm_min  = time->tm_min;
    rtc_time.tm_sec  = time->tm_sec;
    rtc_time.tm_wday = time->tm_wday;
    rtc_time.tm_yday = time->tm_yday;

    RTCDEV(dev)->arg = arg;
    RTCDEV(dev)->callback = callback;
    csi_error_t ret = csi_rtc_set_alarm(&RTCDEV(dev)->handle, (const csi_rtc_time_t *)&rtc_time, _rtc_callback, dev);
    if (ret != CSI_OK) {
        LOGE(TAG, "csi_rtc_set_alarm error");
        return -1;
    }
    return 0;
}

int _rtc_cancel_alarm(rvm_dev_t *dev)
{
    csi_error_t ret = csi_rtc_cancel_alarm(&RTCDEV(dev)->handle);
    if (ret != CSI_OK) {
        LOGE(TAG, "csi_rtc_cancel_alarm error");
        return -1;
    }
    return 0;
}

static rtc_driver_t rtc_driver = {
    .drv = {
        .name   = "rtc",
        .init   = _rtc_init,
        .uninit = _rtc_uninit,
        .open   = _rtc_open,
        .close  = _rtc_close,
        .lpm    = _rtc_lpm,
        .clk_en = _rtc_clock
    },
    .set_time                   = _rtc_set_time,
    .get_time                   = _rtc_get_time,
    .get_alarm_remaining_time   = _rtc_get_alarm_remaining_time,
    .set_alarm                  = _rtc_set_alarm,
    .cancel_alarm               = _rtc_cancel_alarm
};

void rvm_rtc_drv_register(int idx)
{
    rvm_driver_register(&rtc_driver.drv, NULL, idx);
}
