/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef _DEVICE_VFS_I2S_H_
#define _DEVICE_VFS_I2S_H_

#ifdef __cplusplus
extern "C" {
#endif
#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/vfs_device.h>

#define I2S_IOC_SET_CONFIG                    (DEV_I2S_IOCTL_CMD_BASE + 1)
#define I2S_IOC_GET_CONFIG                    (DEV_I2S_IOCTL_CMD_BASE + 2)
#define I2S_IOC_PAUSE                         (DEV_I2S_IOCTL_CMD_BASE + 3)
#define I2S_IOC_RESUME                        (DEV_I2S_IOCTL_CMD_BASE + 4)
#define I2S_IOC_STOP                          (DEV_I2S_IOCTL_CMD_BASE + 5)

#endif
#ifdef __cplusplus
}
#endif

#endif
