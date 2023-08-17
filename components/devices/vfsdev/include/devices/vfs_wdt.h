/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef _DEVICE_VFS_WDT_H_
#define _DEVICE_VFS_WDT_H_

#ifdef __cplusplus
extern "C" {
#endif

#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/vfs_device.h>

#define WDT_IOC_SET_TIMEOUT               (DEV_WDT_IOCTL_CMD_BASE + 1)
#define WDT_IOC_START                     (DEV_WDT_IOCTL_CMD_BASE + 2)
#define WDT_IOC_STOP                      (DEV_WDT_IOCTL_CMD_BASE + 3)
#define WDT_IOC_FEED                      (DEV_WDT_IOCTL_CMD_BASE + 4)
#define WDT_IOC_GET_REMAINING_TIME        (DEV_WDT_IOCTL_CMD_BASE + 5)
#define WDT_IOC_ATTACH_CALLBACK           (DEV_WDT_IOCTL_CMD_BASE + 6)
#define WDT_IOC_DETACH_CALLBACK           (DEV_WDT_IOCTL_CMD_BASE + 7)

typedef struct {
    rvm_hal_wdt_callback callback;
    void *arg;
} rvm_wdt_dev_msg_t;
#endif

#ifdef __cplusplus
}
#endif

#endif
