/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef _DEVICE_VFS_GPRS_MODULE_H
#define _DEVICE_VFS_GPRS_MODULE_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/vfs_device.h>

#define GPRS_IOC_SET_MODE                       (DEV_GPRS_IOCTL_CMD_BASE + 1)
#define GPRS_IOC_GET_MODE                       (DEV_GPRS_IOCTL_CMD_BASE + 2)
#define GPRS_IOC_RESET                          (DEV_GPRS_IOCTL_CMD_BASE + 3)
#define GPRS_IOC_START                          (DEV_GPRS_IOCTL_CMD_BASE + 4)
#define GPRS_IOC_STOP                           (DEV_GPRS_IOCTL_CMD_BASE + 5)
#define GPRS_IOC_SET_IF_GONFIG                  (DEV_GPRS_IOCTL_CMD_BASE + 6)
#define GPRS_IOC_MODULE_INIT_CHECK              (DEV_GPRS_IOCTL_CMD_BASE + 7)
#define GPRS_IOC_CONNECT_TO_GPRS                (DEV_GPRS_IOCTL_CMD_BASE + 8)
#define GPRS_IOC_DISCONNECT_FROM_GPRS           (DEV_GPRS_IOCTL_CMD_BASE + 9)
#define GPRS_IOC_GET_LINK_STATUS                (DEV_GPRS_IOCTL_CMD_BASE + 10)
#define GPRS_IOC_GET_IPADDR                     (DEV_GPRS_IOCTL_CMD_BASE + 11)
#define GPRS_IOC_SMS_SEND                       (DEV_GPRS_IOCTL_CMD_BASE + 12)
#define GPRS_IOC_SMS_SET_CB                     (DEV_GPRS_IOCTL_CMD_BASE + 13)
#define GPRS_IOC_GET_IMSI                       (DEV_GPRS_IOCTL_CMD_BASE + 14)
#define GPRS_IOC_GET_IMEI                       (DEV_GPRS_IOCTL_CMD_BASE + 15)
#define GPRS_IOC_GET_CSQ                        (DEV_GPRS_IOCTL_CMD_BASE + 16)
#define GPRS_IOC_GET_SIMCARD_INFO               (DEV_GPRS_IOCTL_CMD_BASE + 17)

typedef struct {
    uint32_t baud;
    uint8_t flow_control;
    char *sca;
    char *da;
    char *content;
    char *imsi;
    char *imei;
    int *csq;
    char ccid[21];
    int *insert;
} rvm_gprs_dev_msg_t;
#endif

#ifdef __cplusplus
}
#endif

#endif /*_WIFI_MODULE*/
