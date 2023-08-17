/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef DEVICE_VFS_DISPLAY_H
#define DEVICE_VFS_DISPLAY_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/vfs_device.h>

#define DISPLAY_IOC_SET_EVENT                   (DEV_DISPLAY_IOCTL_CMD_BASE + 1)
#define DISPLAY_IOC_GET_INFO                    (DEV_DISPLAY_IOCTL_CMD_BASE + 2)
#define DISPLAY_IOC_GET_FRAMEBUFFER             (DEV_DISPLAY_IOCTL_CMD_BASE + 3)
#define DISPLAY_IOC_SET_BRIGHTNESS              (DEV_DISPLAY_IOCTL_CMD_BASE + 4)
#define DISPLAY_IOC_GET_BRIGHTNESS              (DEV_DISPLAY_IOCTL_CMD_BASE + 5)
#define DISPLAY_IOC_WRITE_AREA                  (DEV_DISPLAY_IOCTL_CMD_BASE + 6)
#define DISPLAY_IOC_WRITE_AREA_ASYNC            (DEV_DISPLAY_IOCTL_CMD_BASE + 7)
#define DISPLAY_IOC_READ_AREA                   (DEV_DISPLAY_IOCTL_CMD_BASE + 8)
#define DISPLAY_IOC_PAN_DISPLAY                 (DEV_DISPLAY_IOCTL_CMD_BASE + 9)
#define DISPLAY_IOC_BLANK_ON_OFF                (DEV_DISPLAY_IOCTL_CMD_BASE + 10)

typedef struct {
    rvm_hal_display_event event_cb;
    void *priv;
    void ***smem_start;
    size_t *smem_len;
    uint8_t *brightness;
    rvm_hal_display_area_t *area;
    void *data;
} rvm_display_dev_msg_t;
#endif

#ifdef __cplusplus
}
#endif

#endif
