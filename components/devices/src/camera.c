/*
 * Copyright (c) 2022, ArtInChip Technology Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Authors: matteo <duanmt@artinchip.com>
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <dev_internal.h>
#include <devices/impl/camera_impl.h>

#define CAMERA_DRIVER(dev)  ((camera_drv_t *)(dev->drv))
#define CAMERA_VAILD(dev) do { \
    if (device_valid(dev, "camera") != 0) \
        return -ENODEV; \
    } while(0)

int rvm_hal_camera_channel_open(rvm_dev_t *dev, rvm_hal_camera_channel_cfg_t *cfg)
{
    int ret = 0;

    CAMERA_VAILD(dev);
    device_lock(dev);
    ret = CAMERA_DRIVER(dev)->channel_open(dev, cfg);
    device_unlock(dev);

    return ret < 0 ? -EIO : 0;
}

int rvm_hal_camera_channel_close(rvm_dev_t *dev, rvm_hal_camera_channel_id_e chn_id)
{
    int ret = 0;

    CAMERA_VAILD(dev);
    device_lock(dev);
    ret = CAMERA_DRIVER(dev)->channel_close(dev, chn_id);
    device_unlock(dev);

    return ret < 0 ? -EIO : 0;
}

int rvm_hal_camera_channel_query(rvm_dev_t *dev, rvm_hal_camera_channel_cfg_t *cfg)
{
    int ret = 0;

    CAMERA_VAILD(dev);
    device_lock(dev);
    ret = CAMERA_DRIVER(dev)->channel_query(dev, cfg);
    device_unlock(dev);

    return ret < 0 ? -EIO : 0;
}

int rvm_hal_camera_channel_start(rvm_dev_t *dev, rvm_hal_camera_channel_id_e chn_id)
{
    int ret = 0;

    CAMERA_VAILD(dev);
    device_lock(dev);
    ret = CAMERA_DRIVER(dev)->channel_start(dev, chn_id);
    device_unlock(dev);

    return ret < 0 ? -EIO : 0;
}

int rvm_hal_camera_channel_stop(rvm_dev_t *dev, rvm_hal_camera_channel_id_e chn_id)
{
    int ret = 0;

    CAMERA_VAILD(dev);
    device_lock(dev);
    ret = CAMERA_DRIVER(dev)->channel_stop(dev, chn_id);
    device_unlock(dev);

    return ret < 0 ? -EIO : 0;
}

int rvm_hal_camera_create_event(rvm_dev_t *dev)
{
    int ret = 0;

    CAMERA_VAILD(dev);
    device_lock(dev);
    ret = CAMERA_DRIVER(dev)->create_event(dev);
    device_unlock(dev);

    return ret < 0 ? -EIO : 0;
}

int rvm_hal_camera_destory_event(rvm_dev_t *dev)
{
    int ret = 0;

    CAMERA_VAILD(dev);
    device_lock(dev);
    ret = CAMERA_DRIVER(dev)->destory_event(dev);
    device_unlock(dev);

    return ret < 0 ? -EIO : 0;
}

int rvm_hal_camera_subscribe_event(rvm_dev_t *dev, rvm_hal_camera_event_subscription_t *subscribe)
{
    int ret = 0;

    CAMERA_VAILD(dev);
    device_lock(dev);
    ret = CAMERA_DRIVER(dev)->subscribe_event(dev, subscribe);
    device_unlock(dev);

    return ret < 0 ? -EIO : 0;
}

int rvm_hal_camera_unsubscribe_event(rvm_dev_t *dev, rvm_hal_camera_event_subscription_t *subscribe)
{
    int ret = 0;

    CAMERA_VAILD(dev);
    device_lock(dev);
    ret = CAMERA_DRIVER(dev)->unsubscribe_event(dev, subscribe);
    device_unlock(dev);

    return ret < 0 ? -EIO : 0;
}

int rvm_hal_camera_get_event(rvm_dev_t *dev, rvm_hal_camera_event_t *event, int timeout_ms)
{
    int ret = 0;

    CAMERA_VAILD(dev);
    device_lock(dev);
    ret = CAMERA_DRIVER(dev)->get_event(dev, event, timeout_ms);
    device_unlock(dev);

    return ret < 0 ? -EIO : 0;
}

int rvm_hal_camera_get_frame_count(rvm_dev_t *dev, rvm_hal_camera_channel_id_e chn_id)
{
    int ret = 0;

    CAMERA_VAILD(dev);
    device_lock(dev);
    ret = CAMERA_DRIVER(dev)->get_frame_count(dev, chn_id);
    device_unlock(dev);

    return ret;
}

int rvm_hal_camera_get_frame(rvm_dev_t *dev, rvm_hal_camera_channel_id_e chn_id, rvm_hal_frame_ex_t *frame, int timeout_ms)
{
    int ret = 0;

    CAMERA_VAILD(dev);
    device_lock(dev);
    ret = CAMERA_DRIVER(dev)->get_frame(dev, chn_id, frame, timeout_ms);
    device_unlock(dev);

    return ret < 0 ? -EIO : 0;
}
