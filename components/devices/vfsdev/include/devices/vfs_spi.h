/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef _DEVICE_VFS_SPI_H_
#define _DEVICE_VFS_SPI_H_

#ifdef __cplusplus
extern "C" {
#endif

#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/vfs_device.h>

#define SPI_IOC_GET_DEFAULT_CONFIG      (DEV_SPI_IOCTL_CMD_BASE + 1)
#define SPI_IOC_CONFIG                  (DEV_SPI_IOCTL_CMD_BASE + 2)
#define SPI_IOC_CONFIG_GET              (DEV_SPI_IOCTL_CMD_BASE + 3)
#define SPI_IOC_SEND                    (DEV_SPI_IOCTL_CMD_BASE + 4)
#define SPI_IOC_RECV                    (DEV_SPI_IOCTL_CMD_BASE + 5)
#define SPI_IOC_SEND_RECV               (DEV_SPI_IOCTL_CMD_BASE + 6)
#define SPI_IOC_SEND_THEN_RECV          (DEV_SPI_IOCTL_CMD_BASE + 7)
#define SPI_IOC_SEND_THEN_SEND          (DEV_SPI_IOCTL_CMD_BASE + 8)

typedef struct {
    uint8_t *tx_data;
    uint8_t *rx_data;
    uint8_t *tx1_data;
    size_t size;
    size_t tx_size;
    size_t tx1_size;
    size_t rx_size;
    uint32_t timeout;
} rvm_spi_dev_msg_t;
#endif

#ifdef __cplusplus
}
#endif

#endif
