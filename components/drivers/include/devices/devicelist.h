/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef YOC_DEVICELIST_H
#define YOC_DEVICELIST_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  register driver of uart
 * @param  [in] idx : index of the uart
 * @return
 */
extern void uart_csky_register(int idx);

/**
 * @brief  register driver of iic
 * @param  [in] idx : index of the iic controller
 * @return
 */
extern void iic_csky_register(int idx);

/**
 * @brief  register driver of eflash
 * @param  [in] idx : 0 is the default
 * @return
 */
extern void flash_csky_register(int idx);

/**
 * @brief  register driver of qsflash
 * @param  [in] idx : 0 is the default
 * @return
 */
extern void spiflash_csky_register(int idx);

/**
 * @brief  register driver of adc
 * @param  [in] idx : index of the adc
 * @return
 */
extern void adc_csky_register(int idx);

/**
 * @brief  register file system of fat
 * @return 0 on success
 */
extern int vfs_fatfs_register(void);

/**
 * @brief  register little file system
 * @param  [in] partition_desc: partion table
 * @return 0 on success
 */
extern int32_t vfs_lfs_register(char *partition_desc);

/**
 * @brief  unregister little file system
 * @return 0 on success
 */
extern int32_t vfs_lfs_unregister(void);

#ifdef __cplusplus
}
#endif

#endif
