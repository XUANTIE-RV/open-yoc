/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef YOC_DEVICELIST_H
#define YOC_DEVICELIST_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void uart_csky_register(int idx);
extern void iic_csky_register(int idx);
extern void flash_csky_register(int idx);
extern void spiflash_csky_register(int idx);

extern int vfs_fatfs_register(void);

#ifdef __cplusplus
}
#endif

#endif
