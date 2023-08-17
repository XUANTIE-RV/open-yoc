/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef DEVICE_VFS_LED_PAT_H
#define DEVICE_VFS_LED_PAT_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/vfs_device.h>

#define LED_IOC_CONTROL               (DEV_LED_IOCTL_CMD_BASE + 1)

typedef struct {
    int color;
    int on_time;
    int off_time;
} rvm_led_dev_msg_t;
#endif

#ifdef __cplusplus
}
#endif

#endif
