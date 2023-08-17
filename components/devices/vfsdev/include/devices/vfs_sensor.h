/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef DEVICE_VFS_SENSOR_PAI_H
#define DEVICE_VFS_SENSOR_PAI_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/vfs_device.h>

#define SENSOR_IOC_FETCH            (DEV_SENSOR_IOCTL_CMD_BASE + 1)
#define SENSOR_IOC_GETVALUE         (DEV_SENSOR_IOCTL_CMD_BASE + 2)

typedef struct {
    size_t size;
    void *value;
} rvm_sensor_dev_msg_t;

#endif

#ifdef __cplusplus
}
#endif

#endif
