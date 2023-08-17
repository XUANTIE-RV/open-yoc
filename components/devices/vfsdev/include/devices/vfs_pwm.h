/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef _DEVICE_VFS_PWM_H_
#define _DEVICE_VFS_PWM_H_

#ifdef __cplusplus
extern "C" {
#endif

#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/vfs_device.h>

#define PWM_IOC_CONFIG                  (DEV_PWM_IOCTL_CMD_BASE + 1)
#define PWM_IOC_CONFIG_GET              (DEV_PWM_IOCTL_CMD_BASE + 2)
#define PWM_IOC_START                   (DEV_PWM_IOCTL_CMD_BASE + 3)
#define PWM_IOC_STOP                    (DEV_PWM_IOCTL_CMD_BASE + 4)

typedef struct {
    rvm_hal_pwm_config_t config;
    uint8_t channel;
} rvm_pwm_dev_msg_t;
#endif

#ifdef __cplusplus
}
#endif

#endif
