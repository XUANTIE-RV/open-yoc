/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef _DEVICE_VFS_CAN_H_
#define _DEVICE_VFS_CAN_H_

#ifdef __cplusplus
extern "C" {
#endif
#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/vfs_device.h>

#define CAN_IOC_SET_CONFIG             (DEV_CAN_IOCTL_CMD_BASE + 1)
#define CAN_IOC_GET_CONFIG             (DEV_CAN_IOCTL_CMD_BASE + 2)
#define CAN_IOC_SET_FILTER_INIT        (DEV_CAN_IOCTL_CMD_BASE + 3)
#define CAN_IOC_SEND                   (DEV_CAN_IOCTL_CMD_BASE + 4)
#define CAN_IOC_RECV                   (DEV_CAN_IOCTL_CMD_BASE + 5)
#define CAN_IOC_SET_EVENT              (DEV_CAN_IOCTL_CMD_BASE + 6)

typedef struct {
    uint32_t timeout;
    rvm_hal_can_msg_t can_msg;
} rvm_can_dev_tx_rx_msg_t;

typedef struct {
    void *arg;
    rvm_hal_can_callback callback;
} rvm_can_dev_set_event_msg_t;
#endif
#ifdef __cplusplus
}
#endif

#endif
