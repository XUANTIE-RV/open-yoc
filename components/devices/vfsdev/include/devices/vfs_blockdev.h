/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef _DEVICE_VFS_BLOCKDEV_H_
#define _DEVICE_VFS_BLOCKDEV_H_

#ifdef __cplusplus
extern "C" {
#endif

#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/vfs_device.h>

#define BLOCKDEV_IOC_GET_INFO               (DEV_BLOCKDEV_IOCTL_CMD_BASE + 1)
#define BLOCKDEV_IOC_ERASE                  (DEV_BLOCKDEV_IOCTL_CMD_BASE + 2)
#define BLOCKDEV_IOC_SELECT_AREA            (DEV_BLOCKDEV_IOCTL_CMD_BASE + 3)
#endif

#ifdef __cplusplus
}
#endif

#endif
