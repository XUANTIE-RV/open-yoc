/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef __FLASH_IMPL_H__
#define __FLASH_IMPL_H__

#include <stdint.h>

#include <devices/driver.h>
#include <devices/flash.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef struct flash_driver {
    driver_t    drv;
    int         (*read)(rvm_dev_t *dev, uint32_t addroff, void *buff, int32_t bytesize);
    int         (*program)(rvm_dev_t *dev, uint32_t dstaddr, const void *srcbuf, int32_t bytesize);
    int         (*erase)(rvm_dev_t *dev, int32_t addroff, int32_t blkcnt);
    int         (*get_info)(rvm_dev_t *dev, rvm_hal_flash_dev_info_t *info);
} flash_driver_t;

#ifdef __cplusplus
}
#endif

#endif
