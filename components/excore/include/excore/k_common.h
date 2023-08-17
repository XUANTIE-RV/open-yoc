/*
 * Copyright (C) 2018-2022 Alibaba Group Holding Limited
 */

#ifndef __K_COMMON_H__
#define __K_COMMON_H__

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "excore/k_mk_config.h"

#ifndef printk
extern int32_t aos_debug_printf(const char *fmt, ...);
#define printk aos_debug_printf
#endif

#ifndef OS_ALIGN_UP
#define OS_ALIGN_UP(x, align)       (((x) + (align) - 1) & ~((align) - 1))
#endif

#ifndef OS_ALIGN_DOWN
#define OS_ALIGN_DOWN(x, align)     ((x) & ~((align) - 1))
#endif

#ifndef min
#define min(a, b)  (((a) < (b)) ? (a) : (b))
#endif

#ifndef max
#define max(a, b)  (((a) > (b)) ? (a) : (b))
#endif

#define K_ARCH_INVALID_ADDR     (unsigned long)(-1)

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof((arr)[0]))
#endif

#ifndef barrier
#define barrier() asm volatile("" ::: "memory")
#endif

#ifndef CHECK_PARAM
#define CHECK_PARAM(x, ret) \
	do { \
		if (!(x)) { \
			return ret; \
		}\
	} while (0)
#endif

#endif /* __K_COMMON_H__ */

