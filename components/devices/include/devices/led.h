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

#ifndef DEVICE_LED_PAT_H
#define DEVICE_LED_PAT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <devices/device.h>

#define COLOR_RED_OFF   16
#define COLOR_GREEN_OFF 8
#define COLOR_BLUE_OFF  0

#define COLOR_SET(r, g, b) (r << COLOR_RED_OFF | g << COLOR_GREEN_OFF | b << COLOR_BLUE_OFF)
#define COLOR_GET(c, off) ((c >> off) & 0xff)

#define COLOR_RED_GET(c)   COLOR_GET(c, COLOR_RED_OFF)
#define COLOR_GREEN_GET(c) COLOR_GET(c, COLOR_GREEN_OFF)
#define COLOR_BLUE_GET(c)  COLOR_GET(c, COLOR_BLUE_OFF)

#define COLOR_RED       COLOR_SET(0xff, 0, 0)   /* 红灯亮 */
#define COLOR_GREEN     COLOR_SET(0, 0xff, 0)   /* 绿灯亮 */
#define COLOR_BLUE      COLOR_SET(0, 0, 0xff)   /* 蓝灯亮 */

#define COLOR_WHITE     COLOR_SET(0xff, 0xff, 0xff) /* 全亮 */
#define COLOR_BLACK     COLOR_SET(0, 0, 0)          /* 全灭 */

#define BLINK_ON        1
#define BLINK_OFF       0

#define LED_PIN_NOT_SET 0xffffffff

/**
 * 打开设备
 * @param name 设备名称
 *             使用rvm_led_rgb_drv_register接口注册YoC通用的LED设备名称前缀固定为"ledrgb"
 *             例如注册调用rvm_led_rgb_drv_register(&led_config, 1)
 *             传入的name为 "ledrgb1"
 * @return 返回设备句柄dev_t
 */
#define rvm_hal_led_open(name) rvm_hal_device_open(name)

/**
 * 关闭设备
 * 由于LED需要长期使用，一般情况下无需关闭
 * @param dev 设备句柄
 */
#define rvm_hal_led_close(dev) rvm_hal_device_close(dev)

/**
 * 控制灯的状态
 * YoC实现的通用驱动,支持GPIO控制RGB三色亮灭,不支持灰度
 * 接口支持灰度保留扩展
 *
 * @param dev 设备句柄
 * @param color 灯亮灭控制，可使用宏COLOR_RED COLOR_GREEN COLOR_BLUE COLOR_GENERIC COLOR_BLACK COLOR_SET
 * @param on_time 亮的持续时间(ms), <=0 闪烁功能无效
 * @param off_time 灭的持续时间(ms), <=0 闪烁功能无效
 */
int rvm_hal_led_control(rvm_dev_t *dev, int color, int on_time, int off_time);


#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/vfs_led.h>
#endif

#ifdef __cplusplus
}
#endif

#endif
