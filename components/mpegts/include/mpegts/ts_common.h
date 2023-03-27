/*
 * Copyright (C) 2018-2022 Alibaba Group Holding Limited
 */

#ifndef __TS_COMMON_H__
#define __TS_COMMON_H__

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <ctype.h>
#include <stdint.h>
#include <sys/time.h>

#ifdef __linux__
#include <errno.h>
#include <pthread.h>
#include "ulog/ulog.h"
#include "aos_port/common.h"
#else
#include <aos/types.h>
#include <aos/aos.h>
#include <aos/debug.h>
#include <aos/kernel.h>
#include <aos/list.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef UNUSED
#define UNUSED(x)
#endif

#ifndef container_of
#define container_of(ptr, type, member) \
        ((type *)((char *)(ptr) - offsetof(type,member)))
#endif

#ifndef MIN
#define MIN(x,y)                ((x) < (y) ? (x) : (y))
#endif

#ifndef MAX
#define MAX(x,y)                ((x) > (y) ? (x) : (y))
#endif

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x)           (sizeof(x) / sizeof((x)[0]))
#endif

#ifdef __cplusplus
}
#endif

#endif /* __TS_COMMON_H__ */

