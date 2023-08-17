/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef DEVICE_VFS_NETDRV_H
#define DEVICE_VFS_NETDRV_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/vfs_device.h>

#define NETDRV_IOC_GET_MAC_ADDR                 (DEV_NETDRV_IOCTL_CMD_BASE + 1)
#define NETDRV_IOC_SET_MAC_ADDR                 (DEV_NETDRV_IOCTL_CMD_BASE + 2)
#define NETDRV_IOC_GET_DNS_SERVER               (DEV_NETDRV_IOCTL_CMD_BASE + 3)
#define NETDRV_IOC_SET_DNS_SERVER               (DEV_NETDRV_IOCTL_CMD_BASE + 4)
#define NETDRV_IOC_SET_LINK_UP                  (DEV_NETDRV_IOCTL_CMD_BASE + 5)
#define NETDRV_IOC_SET_LINK_DOWN                (DEV_NETDRV_IOCTL_CMD_BASE + 6)
#define NETDRV_IOC_SET_HOSTNAME                 (DEV_NETDRV_IOCTL_CMD_BASE + 7)
#define NETDRV_IOC_GET_HOSTNAME                 (DEV_NETDRV_IOCTL_CMD_BASE + 8)
#define NETDRV_IOC_START_DHCP                   (DEV_NETDRV_IOCTL_CMD_BASE + 9)
#define NETDRV_IOC_STOP_DHCP                    (DEV_NETDRV_IOCTL_CMD_BASE + 10)
#define NETDRV_IOC_SET_IPADDR                   (DEV_NETDRV_IOCTL_CMD_BASE + 11)
#define NETDRV_IOC_GET_IPADDR                   (DEV_NETDRV_IOCTL_CMD_BASE + 12)
#define NETDRV_IOC_PING                         (DEV_NETDRV_IOCTL_CMD_BASE + 13)
#define NETDRV_IOC_SUBSCRIBE                    (DEV_NETDRV_IOCTL_CMD_BASE + 14)
#define NETDRV_IOC_UNSUBSCRIBE                  (DEV_NETDRV_IOCTL_CMD_BASE + 15)

typedef struct {
    ip_addr_t ipaddr;
    ip_addr_t netmask;
    ip_addr_t gw;
    int type;
    char *remote_ip;
    uint32_t event;
    event_callback_t cb;
    void *param;
} rvm_netdrv_dev_msg_t;

typedef struct {
    uint32_t num;
    ip_addr_t *ipaddr;
} rvm_netdrv_dev_dns_server_msg_t;

#endif

#ifdef __cplusplus
}
#endif

#endif
