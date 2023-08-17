/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef DEVICES_VFS_HCI_H_
#define DEVICES_VFS_HCI_H_

#ifdef __cplusplus
extern "C" {
#endif

#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/vfs_device.h>

#define HCI_IOC_SET_EVENT             (DEV_HCI_IOCTL_CMD_BASE + 1)
#define HCI_IOC_START                 (DEV_HCI_IOCTL_CMD_BASE + 2)
#define HCI_IOC_SEND                  (DEV_HCI_IOCTL_CMD_BASE + 3)
#define HCI_IOC_RECV                  (DEV_HCI_IOCTL_CMD_BASE + 4)

typedef struct {
    hci_event_cb_t event;
    void *data;
    uint32_t size;
    void *priv;
} rvm_hci_dev_msg_t;
#endif

#ifdef __cplusplus
}
#endif

#endif
