/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef DEVICE_VFS_ADC_H
#define DEVICE_VFS_ADC_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/vfs_device.h>

#define ADC_IOC_GET_DEFAULT_CONFIG      (DEV_ADC_IOCTL_CMD_BASE + 1)
#define ADC_IOC_SET_CONFIG              (DEV_ADC_IOCTL_CMD_BASE + 2)
#define ADC_IOC_GET_PIN2CHANNEL         (DEV_ADC_IOCTL_CMD_BASE + 3)
#define ADC_IOC_DMA_ENABLE              (DEV_ADC_IOCTL_CMD_BASE + 4)
#define ADC_IOC_GET_VALUE               (DEV_ADC_IOCTL_CMD_BASE + 5)
#define ADC_IOC_GET_MULTIPLE_VALUE      (DEV_ADC_IOCTL_CMD_BASE + 6)

typedef struct {
    uint8_t ch;
    size_t num;
    uint8_t pin;
    uint32_t timeout;
    void *output;
} rvm_adc_dev_msg_t;
#endif

#ifdef __cplusplus
}
#endif

#endif
