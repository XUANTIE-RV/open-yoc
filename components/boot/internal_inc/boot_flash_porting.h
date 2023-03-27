/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#ifndef __BOOT_FLASH_H__
#define __BOOT_FLASH_H__

#include <stdint.h>
#include <stddef.h>
#include <yoc/partition_device.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef partition_device_info_t boot_device_info_t;

int boot_device_info_get(storage_info_t *storage_info, boot_device_info_t *info);
int boot_device_read(storage_info_t *storage_info, uint32_t offset, void *data, size_t data_len);
int boot_device_write(storage_info_t *storage_info, uint32_t offset, void *data, size_t data_len);
int boot_device_erase(storage_info_t *storage_info, uint32_t offset, size_t len);

#ifdef __cplusplus
}
#endif
#endif