/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#ifndef DEVICE_VFS_DEVICE_H
#define DEVICE_VFS_DEVICE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <devfs.h>

#define DEV_ADC_IOCTL_CMD_BASE          0x1000
#define DEV_BATTERY_IOCTL_CMD_BASE      0x1100
#define DEV_BLOCKDEV_IOCTL_CMD_BASE     0x1200
#define DEV_CLK_IOCTL_CMD_BASE          0x1300
#define DEV_DISPLAY_IOCTL_CMD_BASE      0x1400
#define DEV_ETH_IOCTL_CMD_BASE          0x1500
#define DEV_FLASH_IOCTL_CMD_BASE        0x1600
#define DEV_GNSS_IOCTL_CMD_BASE         0x1700
#define DEV_GPIOPIN_IOCTL_CMD_BASE      0x1800
#define DEV_GPRS_IOCTL_CMD_BASE         0x1900
#define DEV_HCI_IOCTL_CMD_BASE          0x1A00
#define DEV_IIC_IOCTL_CMD_BASE          0x1B00
#define DEV_INPUT_IOCTL_CMD_BASE        0x1C00
#define DEV_LED_IOCTL_CMD_BASE          0x1D00
#define DEV_NBIOT_IOCTL_CMD_BASE        0x1E00
#define DEV_NETDRV_IOCTL_CMD_BASE       0x1F00
#define DEV_RTC_IOCTL_CMD_BASE          0x2000
#define DEV_SENSOR_IOCTL_CMD_BASE       0x2100
#define DEV_UART_IOCTL_CMD_BASE         0x2200
#define DEV_WDT_IOCTL_CMD_BASE          0x2300
#define DEV_WIFI_IOCTL_CMD_BASE         0x2400
#define DEV_TIMER_IOCTL_CMD_BASE        0x2500
#define DEV_PWM_IOCTL_CMD_BASE          0x2600
#define DEV_SPI_IOCTL_CMD_BASE          0x2700
#define DEV_CAN_IOCTL_CMD_BASE          0x2800
#define DEV_CIR_IOCTL_CMD_BASE          0x2900
#define DEV_QSPI_IOCTL_CMD_BASE         0x3000
#define DEV_I2S_IOCTL_CMD_BASE          0x3100



/* ld sections declaration */
#if defined (__CC_ARM) || defined (__GNUC__)
#define OS_VFS_DRIVER_ATT     __attribute__((used, section(".vfs_driver_entry")))
#elif defined (__ICCARM__)
#pragma OS_VFS_DRIVER_ATT    = ".vfs_driver_entry"
#endif

typedef void (*OS_DRIVER_ENTRY)(void);
extern int rvm_vfs_device_drv_init(OS_DRIVER_ENTRY drv_init_entry);
#define VFS_DEV_DRIVER_ENTRY(init)     \
    void vfs_drv_##init(void)          \
    {                                  \
        rvm_vfs_device_drv_init(init); \
    }                                  \
    OS_VFS_DRIVER_ATT OS_DRIVER_ENTRY vfs_##init = vfs_drv_##init;

typedef struct {
    slist_t node;
    char name[DEVFS_NODE_NAME_MAX_LEN + 1];
    devfs_file_ops_t *ops;
} devfs_ops_node_t;

void devices_add_devfs_ops_node(const devfs_ops_node_t *ops_node);

#ifdef __cplusplus
}
#endif

#endif

#endif