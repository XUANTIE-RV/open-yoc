/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef _DEVICE_VFS_ETH_H_
#define _DEVICE_VFS_ETH_H_

#ifdef __cplusplus
extern "C" {
#endif

#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/vfs_device.h>

#define ETH_IOC_RESET                   (DEV_ETH_IOCTL_CMD_BASE + 1)
#define ETH_IOC_START                   (DEV_ETH_IOCTL_CMD_BASE + 2)
#define ETH_IOC_STOP                    (DEV_ETH_IOCTL_CMD_BASE + 3)
#define ETH_IOC_MAC_CONTROL             (DEV_ETH_IOCTL_CMD_BASE + 4)
#define ETH_IOC_SET_PACKET_FILTER       (DEV_ETH_IOCTL_CMD_BASE + 5)

#endif

#ifdef __cplusplus
}
#endif

#endif
