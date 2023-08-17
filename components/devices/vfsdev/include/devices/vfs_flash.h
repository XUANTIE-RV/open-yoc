/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef DEVICE_VFS_FLASH_PAI_H
#define DEVICE_VFS_FLASH_PAI_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/vfs_device.h>

#define FLASH_IOC_GET_INFO              (DEV_FLASH_IOCTL_CMD_BASE + 1)
#define FLASH_IOC_ERASE                 (DEV_FLASH_IOCTL_CMD_BASE + 2)
#endif

#ifdef __cplusplus
}
#endif

#endif
