/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef DEVICE_VFS_BATTERY_H
#define DEVICE_VFS_BATTERY_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/vfs_device.h>

#define BATTERY_IOC_FETCH               (DEV_BATTERY_IOCTL_CMD_BASE + 1)
#define BATTERY_IOC_GETVALUE            (DEV_BATTERY_IOCTL_CMD_BASE + 2)
#define BATTERY_IOC_EVENT_CB            (DEV_BATTERY_IOCTL_CMD_BASE + 3)

typedef struct {
    rvm_hal_battery_attr_t attr;
    void *value;
    size_t size;
} rvm_battery_dev_msg_t;
#endif

#ifdef __cplusplus
}
#endif

#endif
