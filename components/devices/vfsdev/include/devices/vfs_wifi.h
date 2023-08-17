/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef DEVICE_VFS_WIFI_H
#define DEVICE_VFS_WIFI_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/vfs_device.h>

#define WIFI_IOC_INIT                               (DEV_WIFI_IOCTL_CMD_BASE + 1)
#define WIFI_IOC_DEINIT                             (DEV_WIFI_IOCTL_CMD_BASE + 2)
#define WIFI_IOC_RESET                              (DEV_WIFI_IOCTL_CMD_BASE + 3)
#define WIFI_IOC_SET_MODE                           (DEV_WIFI_IOCTL_CMD_BASE + 4)
#define WIFI_IOC_GET_MODE                           (DEV_WIFI_IOCTL_CMD_BASE + 5)
#define WIFI_IOC_INSTALL_EVENT_CB                   (DEV_WIFI_IOCTL_CMD_BASE + 6)
#define WIFI_IOC_SET_PROTOCOL                       (DEV_WIFI_IOCTL_CMD_BASE + 7)
#define WIFI_IOC_GET_PROTOCOL                       (DEV_WIFI_IOCTL_CMD_BASE + 8)
#define WIFI_IOC_SET_COUNTRY                        (DEV_WIFI_IOCTL_CMD_BASE + 9)
#define WIFI_IOC_GET_COUNTRY                        (DEV_WIFI_IOCTL_CMD_BASE + 10)
#define WIFI_IOC_SET_MAC_ADDR                       (DEV_WIFI_IOCTL_CMD_BASE + 11)
#define WIFI_IOC_GET_MAC_ADDR                       (DEV_WIFI_IOCTL_CMD_BASE + 12)
#define WIFI_IOC_SET_AUTO_RECONNECT                 (DEV_WIFI_IOCTL_CMD_BASE + 13)
#define WIFI_IOC_GET_AUTO_RECONNECT                 (DEV_WIFI_IOCTL_CMD_BASE + 14)
#define WIFI_IOC_SET_LPM                            (DEV_WIFI_IOCTL_CMD_BASE + 15)
#define WIFI_IOC_GET_LPM                            (DEV_WIFI_IOCTL_CMD_BASE + 16)
#define WIFI_IOC_POWER_ON                           (DEV_WIFI_IOCTL_CMD_BASE + 17)
#define WIFI_IOC_POWER_OFF                          (DEV_WIFI_IOCTL_CMD_BASE + 18)
#define WIFI_IOC_START_SCAN                         (DEV_WIFI_IOCTL_CMD_BASE + 19)
#define WIFI_IOC_START                              (DEV_WIFI_IOCTL_CMD_BASE + 20)
#define WIFI_IOC_STOP                               (DEV_WIFI_IOCTL_CMD_BASE + 21)
#define WIFI_IOC_STA_GET_LINK_STATUS                (DEV_WIFI_IOCTL_CMD_BASE + 22)
#define WIFI_IOC_AP_GET_STA_LIST                    (DEV_WIFI_IOCTL_CMD_BASE + 23)
#define WIFI_IOC_START_MONITOR                      (DEV_WIFI_IOCTL_CMD_BASE + 24)
#define WIFI_IOC_STOP_MONITOR                       (DEV_WIFI_IOCTL_CMD_BASE + 25)
#define WIFI_IOC_START_MGNT_MONITOR                 (DEV_WIFI_IOCTL_CMD_BASE + 26)
#define WIFI_IOC_STOP_MGNT_MONITOR                  (DEV_WIFI_IOCTL_CMD_BASE + 27)
#define WIFI_IOC_SEND_80211_RAW_FRAME               (DEV_WIFI_IOCTL_CMD_BASE + 28)
#define WIFI_IOC_SET_CHANNEL                        (DEV_WIFI_IOCTL_CMD_BASE + 29)
#define WIFI_IOC_GET_CHANNEL                        (DEV_WIFI_IOCTL_CMD_BASE + 30)
#define WIFI_IOC_SET_SMARTCFG                       (DEV_WIFI_IOCTL_CMD_BASE + 31)

typedef struct {
    uint8_t *protocol_bitmap;
    uint8_t *mac;
    bool *en;
    wifi_scan_config_t *config;
    bool block;
    void *buffer;
    uint16_t len;
    uint8_t *primary;
    rvm_hal_wifi_second_chan_t *second;
} rvm_wifi_dev_msg_t;
#endif

#ifdef __cplusplus
}
#endif

#endif

