/*
 * Copyright (C) 2020 FishSemi Inc. All rights reserved.
 */

#ifndef _DEVICE_VFS_NBIOT_MODULE_H
#define _DEVICE_VFS_NBIOT_MODULE_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/vfs_device.h>

#define NBIOT_IOC_START                             (DEV_NBIOT_IOCTL_CMD_BASE + 1)
#define NBIOT_IOC_STOP                              (DEV_NBIOT_IOCTL_CMD_BASE + 2)
#define NBIOT_IOC_GET_STATUS                        (DEV_NBIOT_IOCTL_CMD_BASE + 3)
#define NBIOT_IOC_GET_IMSI                          (DEV_NBIOT_IOCTL_CMD_BASE + 4)
#define NBIOT_IOC_GET_IMEI                          (DEV_NBIOT_IOCTL_CMD_BASE + 5)
#define NBIOT_IOC_GET_CSQ                           (DEV_NBIOT_IOCTL_CMD_BASE + 6)
#define NBIOT_IOC_GET_SIMCARD_INFO                  (DEV_NBIOT_IOCTL_CMD_BASE + 7)
#define NBIOT_IOC_GET_CELL_INFO                     (DEV_NBIOT_IOCTL_CMD_BASE + 8)
#define NBIOT_IOC_SET_STATUS_IND                    (DEV_NBIOT_IOCTL_CMD_BASE + 9)
#define NBIOT_IOC_SET_SIGNAL_STRENGTH_IND           (DEV_NBIOT_IOCTL_CMD_BASE + 10)

typedef struct {
    rvm_hal_nbiot_iccid_t *iccid;
    int *insert;
} rvm_nbiot_dev_msg_t;
#endif
#ifdef __cplusplus
}
#endif

#endif /*_DEVICE_NBIOT_MODULE_H*/
