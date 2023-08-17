/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef _DEVICE_VFS_QSPI_H_
#define _DEVICE_VFS_QSPI_H_

#ifdef __cplusplus
extern "C" {
#endif


#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/vfs_device.h>

#define QSPI_IOC_SET_CONFIG             (DEV_QSPI_IOCTL_CMD_BASE + 1)
#define QSPI_IOC_GET_CONFIG             (DEV_QSPI_IOCTL_CMD_BASE + 2)
#define QSPI_IOC_SEND                   (DEV_QSPI_IOCTL_CMD_BASE + 3)
#define QSPI_IOC_RECV                   (DEV_QSPI_IOCTL_CMD_BASE + 4)
#define QSPI_IOC_SEND_AND_RECV          (DEV_QSPI_IOCTL_CMD_BASE + 5)

typedef struct
{
    uint8_t *data;
    size_t size;
    uint8_t *tx_data;
    uint8_t *rx_data;
    uint32_t timeout;
    rvm_hal_qspi_cmd_t cmd;
} rvm_qspi_dev_msg_t;

#endif

#ifdef __cplusplus
}
#endif

#endif
