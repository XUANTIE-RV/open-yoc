/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#ifndef __BOOT_FLASH_H__
#define __BOOT_FLASH_H__

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    unsigned long    start_addr;
    uint32_t    sector_size;
    uint32_t    sector_count;
} boot_flash_info_t;

int boot_flash_info_get(int flashid, boot_flash_info_t *info);

int boot_flash_read(unsigned long addr, void *data, size_t data_len);

int boot_flash_write(unsigned long addr, void *data, size_t data_len);

int boot_flash_erase(unsigned long addr, size_t len);

#ifdef __cplusplus
}
#endif
#endif