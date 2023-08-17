/*
 * Copyright (C) 2023 Alibaba Group Holding Limited
 */

#ifndef _DEVICE_VFS_TIMER_H_
#define _DEVICE_VFS_TIMER_H_

#ifdef __cplusplus
extern "C" {
#endif

#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/vfs_device.h>

#define TIMER_IOC_START                     (DEV_TIMER_IOCTL_CMD_BASE + 1)
#define TIMER_IOC_STOP                      (DEV_TIMER_IOCTL_CMD_BASE + 2)
#define TIMER_IOC_ATTACH_CALLBACK           (DEV_TIMER_IOCTL_CMD_BASE + 3)
#define TIMER_IOC_DETACH_CALLBACK           (DEV_TIMER_IOCTL_CMD_BASE + 4)
#define TIMER_IOC_GET_REMAINING_VALUE       (DEV_TIMER_IOCTL_CMD_BASE + 5)
#define TIMER_IOC_GET_LOAD_VALUE            (DEV_TIMER_IOCTL_CMD_BASE + 6)

typedef struct {
    rvm_hal_timer_callback callback;
    void *arg;
} rvm_timer_dev_msg_t;
#endif

#ifdef __cplusplus
}
#endif

#endif
