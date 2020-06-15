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
    int (*master_send)(aos_dev_t *dev, uint16_t dev_addr, const void *data, uint32_t size, uint32_t timeout);
    int (*master_recv)(aos_dev_t *dev, uint16_t dev_addr, void *data, uint32_t size, uint32_t timeout);
    int (*slave_send)(aos_dev_t *dev, const void *data, uint32_t size, uint32_t timeout);
    int (*slave_recv)(aos_dev_t *dev, void *data, uint32_t size, uint32_t timeout);
    int (*mem_write)(aos_dev_t *dev, uint16_t dev_addr, uint16_t mem_addr, uint16_t mem_addr_size,\
                        const void *data, uint32_t size, uint32_t timeout);
    int (*mem_read)(aos_dev_t *dev, uint16_t dev_addr, uint16_t mem_addr, uint16_t mem_addr_size,\
                        void *data, uint32_t size, uint32_t timeout);
} iic_driver_t;

#ifdef __cplusplus
}
#endif

#endif