/*
 * Copyright (c) 2017 Linaro Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __BLE_TYPES_H__
#define __BLE_TYPES_H__

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

#ifndef bool
#define bool unsigned char
#endif

typedef signed char         s8_t;
typedef signed short        s16_t;
typedef signed int          s32_t;
typedef signed long long    s64_t;

typedef unsigned char       u8_t;
typedef unsigned short      u16_t;
typedef unsigned int        u32_t;
typedef unsigned long long  u64_t;

#ifndef _SSIZE_T_DECLARED
typedef long ssize_t;
#define _SSIZE_T_DECLARED
#endif

#ifndef _OFF_T_DECLARED
typedef int off_t;
#define _OFF_T_DECLARED
#endif

typedef s32_t k_timeout_t;

#define __deprecated

#ifdef __packed
#undef __packed
#define __packed __attribute__((packed))
#endif

#ifdef __cplusplus
}
#endif

#endif /* __Z_TYPES_H__ */
