/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
/******************************************************************************
 * @file     sys_freq.h
 * @brief    header file for setting system frequency.
 * @version  V1.0
 * @date     18. July 2018
 ******************************************************************************/
#ifndef _SYS_FREQ_H_
#define _SYS_FREQ_H_

#include <stdint.h>
#include "soc.h"

int32_t drv_get_sys_freq(void);
int32_t drv_get_timer_freq(void);
uint32_t drv_get_cur_cpu_freq(void);

#endif /* _SYS_FREQ_H_ */

