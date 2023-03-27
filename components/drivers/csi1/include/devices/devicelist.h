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
extern void rvm_uart_drv_register(int idx);

/**
 * @brief  register driver of iic
 * @param  [in] idx : index of the iic controller
 * @return
 */
extern void rvm_iic_drv_register(int idx);

/**
 * @brief  register driver of eflash
 * @param  [in] idx : 0 is the default
 * @return
 */
extern void rvm_eflash_drv_register(int idx);

/**
 * @brief  register driver of qsflash
 * @param  [in] idx : 0 is the default
 * @return
 */
extern void rvm_spiflash_drv_register(int idx);

/**
 * @brief  register driver of adc
 * @param  [in] idx : index of the adc
 * @return
 */
extern void rvm_adc_drv_register(int idx);

#ifdef __cplusplus
}
#endif

#endif
