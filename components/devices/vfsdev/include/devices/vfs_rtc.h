/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef _DEVICE_VFS_RTC_H_
#define _DEVICE_VFS_RTC_H_

#ifdef __cplusplus
extern "C" {
#endif

#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/vfs_device.h>

#define RTC_IOC_SET_TIME                    (DEV_RTC_IOCTL_CMD_BASE + 1)
#define RTC_IOC_GET_TIME                    (DEV_RTC_IOCTL_CMD_BASE + 2)
#define RTC_IOC_GET_ALARM_REMAINING_TIME    (DEV_RTC_IOCTL_CMD_BASE + 3)
#define RTC_IOC_SET_ALARM                   (DEV_RTC_IOCTL_CMD_BASE + 4)
#define RTC_IOC_CANCEL_ALARM                (DEV_RTC_IOCTL_CMD_BASE + 5)

typedef struct {
    struct tm *time;
    rvm_hal_rtc_callback callback;
    void *arg;
} rvm_rtc_dev_msg_t;
#endif

#ifdef __cplusplus
}
#endif

#endif
