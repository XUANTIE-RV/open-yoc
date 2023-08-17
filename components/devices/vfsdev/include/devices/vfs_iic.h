/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef DEVICE_VFS_IIC_PAI_H
#define DEVICE_VFS_IIC_PAI_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/vfs_device.h>

#define IIC_IOC_GET_DEFAULT_CONFIG      (DEV_IIC_IOCTL_CMD_BASE + 1)
#define IIC_IOC_SET_CONFIG              (DEV_IIC_IOCTL_CMD_BASE + 2)
#define IIC_IOC_DMA_ENABLE              (DEV_IIC_IOCTL_CMD_BASE + 3)
#define IIC_IOC_MASTER_SEND             (DEV_IIC_IOCTL_CMD_BASE + 4)
#define IIC_IOC_MASTER_RECV             (DEV_IIC_IOCTL_CMD_BASE + 5)
#define IIC_IOC_SLAVE_SEND              (DEV_IIC_IOCTL_CMD_BASE + 6)
#define IIC_IOC_SLAVE_RECV              (DEV_IIC_IOCTL_CMD_BASE + 7)
#define IIC_IOC_MEM_WRITE               (DEV_IIC_IOCTL_CMD_BASE + 8)
#define IIC_IOC_MEM_READ                (DEV_IIC_IOCTL_CMD_BASE + 9)

typedef struct {
    uint16_t dev_addr;
    void *data;
    uint32_t size;
    uint32_t timeout;
    uint16_t mem_addr;
    uint16_t mem_addr_size;
} rvm_iic_dev_msg_t;
#endif

#ifdef __cplusplus
}
#endif

#endif
