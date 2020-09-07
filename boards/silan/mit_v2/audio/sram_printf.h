/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     sram_printf.h
 * @brief    sram printf header
 * @version  V1.0
 * @date     28. June 2019
 ******************************************************************************/

#ifndef _SRAM_PRINTF_H_
#define _SRAM_PRINTF_H_

#include <stdarg.h>

int sram_printf(const char *format, ...);

#endif  /* _SRAM_PRINTF_H_ */
