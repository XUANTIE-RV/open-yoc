/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef _DEVICE_VFS_CLK_H_
#define _DEVICE_VFS_CLK_H_

#ifdef __cplusplus
extern "C" {
#endif

#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/vfs_device.h>

#define CLK_IOC_ENABLE          (DEV_CLK_IOCTL_CMD_BASE + 1)
#define CLK_IOC_DISABLE         (DEV_CLK_IOCTL_CMD_BASE + 2)
#define CLK_IOC_GET_FREQ        (DEV_CLK_IOCTL_CMD_BASE + 3)
#define CLK_IOC_SET_FREQ        (DEV_CLK_IOCTL_CMD_BASE + 4)

typedef struct {
    rvm_hal_clk_id_t clk_id;
    uint32_t idx;
    uint32_t freq;
} rvm_clk_dev_msg_t;
#endif

#ifdef __cplusplus
}
#endif

#endif
