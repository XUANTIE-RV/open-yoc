/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef DEVICE_VFS_INPUT_H
#define DEVICE_VFS_INPUT_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/vfs_device.h>

#define INPUT_IOC_SET_EVENT               (DEV_INPUT_IOCTL_CMD_BASE + 1)
#define INPUT_IOC_UNSET_EVENT             (DEV_INPUT_IOCTL_CMD_BASE + 2)
#define INPUT_IOC_READ_TIMEOUT_MS         (DEV_INPUT_IOCTL_CMD_BASE + 3)

typedef struct {
    rvm_hal_input_event event_cb;
    void *priv;
} rvm_input_dev_msg_t;

#endif

#ifdef __cplusplus
}
#endif

#endif
