/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef HAL_IIC_IMPL_H
#define HAL_IIC_IMPL_H

#include <stdint.h>

#include <devices/driver.h>
#include <devices/iic.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct iic_driver {
    driver_t drv;
    int (*config)(aos_dev_t *dev, iic_config_t *config);
    int (*send)(aos_dev_t *dev, uint8_t dev_addr, const void *data, uint32_t size);
    int (*recv)(aos_dev_t *dev, uint8_t dev_addr, void *data, uint32_t size);
} iic_driver_t;

#ifdef __cplusplus
}
#endif

#endif