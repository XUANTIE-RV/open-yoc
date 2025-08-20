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

#ifndef DEVICE_DISPLAY_H
#define DEVICE_DISPLAY_H

#include <stdio.h>
#include <stdlib.h>

#include <aos/aos.h>
#include <devices/device.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BIT(n) (1UL << (n))

/** @brief display event id */
#define DISPLAY_EVENT_WRITE_DONE 0          /* EVENT for rvm_hal_display_write_area_async */
#define DISPLAY_EVENT_FLUSH_DONE 1          /* EVENT for rvm_hal_display_pan_display */

/** @brief display feature enum */
typedef enum {
    DISPLAY_FEATURE_WRITE_ASYNC = BIT(0),  /* display support rvm_hal_display_write_area_async */
    DISPLAY_FEATURE_ONE_FB      = BIT(1),  /* display support single framebuffer */
    DISPLAY_FEATURE_DOUBLE_FB   = BIT(2),  /* display support double framebuffer */
} rvm_hal_display_feature_t;

/** @brief display pixel format */
typedef enum {
    DISPLAY_PIXEL_FORMAT_ARGB_8888 = 0,
    DISPLAY_PIXEL_FORMAT_ABGR_8888 = 1,
    DISPLAY_PIXEL_FORMAT_RGB_888   = 2,
    DISPLAY_PIXEL_FORMAT_BGR_888   = 3,
    DISPLAY_PIXEL_FORMAT_RGB_565   = 4,
    DISPLAY_PIXEL_FORMAT_RGB_655   = 5,
    DISPLAY_PIXEL_FORMAT_RGB_556   = 6,
    DISPLAY_PIXEL_FORMAT_BGR_565   = 7,
    DISPLAY_PIXEL_FORMAT_ARGB_1555 = 8,
    DISPLAY_PIXEL_FORMAT_ARGB_4444 = 9,
    DISPLAY_PIXEL_FORMAT_RGB_444   = 10,
    DISPLAY_PIXEL_FORMAT_RGB_332   = 11,
    DISPLAY_PIXEL_FORMAT_RGB_233   = 12,
    DISPLAY_PIXEL_FORMAT_RGB_323   = 13,
    DISPLAY_PIXEL_FORMAT_MONO_8    = 14,
    DISPLAY_PIXEL_FORMAT_MONO_4    = 15,
    DISPLAY_PIXEL_FORMAT_MONO_1    = 16,
} rvm_hal_display_pixel_format_t;

/** @brief display infomation */
typedef struct {
    /** resolution in the X direction  */
    uint32_t x_res;
    /** resolution in the Y direction  */
    uint32_t y_res;
    /** bits per pixel */
    uint32_t bits_per_pixel;
    /** Currently active pixel format for the display */
    rvm_hal_display_pixel_format_t pixel_format;
    /** display support feature \ref rvm_hal_display_feature_t */
    uint32_t supported_feature;
    /** framebuffer start addr */
    void *smem_start[2];
    /** framebuffer size */
    uint32_t smem_len;
} rvm_hal_display_info_t;

/** @brief area infomation for read write */
typedef struct {
    /** start point in the X direction */
    uint32_t x_start;
    /** start point in the Y direction */
    uint32_t y_start;
    /** end point in the X direction */
    uint32_t x_leght;
    /** end point in the Y direction */
    uint32_t y_leght;
} rvm_hal_display_area_t;

/** @brief display event callback */
typedef void (*rvm_hal_display_event)(rvm_dev_t *dev, int event_id, void *priv);

#define rvm_hal_display_open(name)        rvm_hal_device_open(name)
#define rvm_hal_display_close(dev)        rvm_hal_device_close(dev)

/**
  \brief       set event callback
  \param[in]   dev      Pointer to device object.
  \param[in]   event_cb event callback
  \param[in]   priv     private data for user
  \return      0 on success, else on fail.
*/
int rvm_hal_display_set_event(rvm_dev_t *dev, rvm_hal_display_event event_cb, void *priv);

/**
  \brief       get display infomation
  \param[in]   dev      Pointer to device object.
  \param[out]  info     display infomation
  \return      0 on success, else on fail.
*/
int rvm_hal_display_get_info(rvm_dev_t *dev, rvm_hal_display_info_t *info);

/**
  \brief       display get framebuffer
  \param[in]   dev        Pointer to device object.
  \param[out]  smem_start framebuffer start addr
  \param[out]  smem_len   framebuffer size
  \return      0 on success, else on fail.
*/
int rvm_hal_display_get_framebuffer(rvm_dev_t *dev, void ***smem_start, size_t *smem_len);

/**
  \brief       display set backlight brightness
  \param[in]   dev        Pointer to device object.
  \param[in]   brightness backlight brightness range: 0 ~ 255
  \return      0 on success, else on fail.
*/
int rvm_hal_display_set_brightness(rvm_dev_t *dev, uint8_t brightness);

/**
  \brief       display get backlight brightness
  \param[in]   dev        Pointer to device object.
  \param[out]  brightness backlight brightness range: 0 ~ 255
  \return      0 on success, else on fail.
*/
int rvm_hal_display_get_brightness(rvm_dev_t *dev, uint8_t *brightness);

/**
  \brief       write data from framebuffer in specific area in synchronous mode
  \param[in]   dev      Pointer to device object.
  \param[in]   area     specific area
  \param[in]   data     buffer data to write
  \return      0 on success, else on fail.
*/
int rvm_hal_display_write_area(rvm_dev_t *dev, rvm_hal_display_area_t *area, void *data);

/**
  \brief       write data from framebuffer in specific area in asynchronous mode,
               will call event_cb with DISPLAY_EVENT_WRITE_DONE after done
  \param[in]   dev      Pointer to device object.
  \param[in]   area     specific area
  \param[in]   data     buffer data to write
  \return      0 on success, else on fail.
*/
int rvm_hal_display_write_area_async(rvm_dev_t *dev, rvm_hal_display_area_t *area, void *data);

/**
  \brief       read data from framebuffer in specific area
  \param[in]   dev      Pointer to device object.
  \param[in]   area     specific area
  \param[in]   data     buffer data pointer to read
  \return      0 on success, else on fail.
*/
int rvm_hal_display_read_area(rvm_dev_t *dev, rvm_hal_display_area_t *area, void *data);

/**
  \brief       display farmebuffer data to pan,
               will call event_cb with DISPLAY_EVENT_FLUSH_DONE after done
  \param[in]   dev      Pointer to device object.
  \return      0 on success, else on fail.
*/
int rvm_hal_display_pan_display(rvm_dev_t *dev);

/**
  \brief       display control on off
  \param[in]   dev         Pointer to device object.
  \param[in]   on_off      1: on, 0: off
  \return      0 on success, else on fail.
*/
int rvm_hal_display_blank_on_off(rvm_dev_t *dev, uint8_t on_off);


#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/vfs_display.h>
#endif

#ifdef __cplusplus
}
#endif

#endif
