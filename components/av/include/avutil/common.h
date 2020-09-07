/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __COMMON_H__
#define __COMMON_H__

//#include <string.h>
//#include <stdlib.h>
//#include <stdio.h>
#include <unistd.h>
//#include <errno.h>
//#include <sys/stat.h>
//#include <sys/types.h>
//#include <stdarg.h>
//#include <stddef.h>
//#include <math.h>
#include <sys/time.h>
//#include <sys/types.h>
//#include <sys/stat.h>
//#include <sys/socket.h>
//#include <fcntl.h>

#include <aos/types.h>
#include <aos/aos.h>
#include <aos/debug.h>
#include <aos/kernel.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int handle_t;

#ifndef INT_MAX
#define INT_MAX     (2147483647)
#endif

#ifndef UNUSED
//#define UNUSED __attribute__((unused))
#define UNUSED(x)
#endif

#ifndef container_of
#define container_of(ptr, type, member) \
        ((type *)((char *)(ptr) - offsetof(type,member)))
#endif /* container_of */

#ifndef MIN
#define MIN(x,y)                ((x) < (y) ? (x) : (y))
#endif

#ifndef MAX
#define MAX(x,y)                ((x) > (y) ? (x) : (y))
#endif

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x)           (sizeof(x) / sizeof((x)[0]))
#endif

#ifndef SEEK_SET
#define SEEK_SET 0
#endif

#ifndef SEEK_CUR
#define SEEK_CUR 1
#endif

#ifndef SEEK_END
#define SEEK_END 2
#endif

#ifndef AV_ALIGN_SIZE
#define AV_ALIGN_SIZE(size, align) (((size) + align - 1U) & (~(align - 1U)))
#endif

#ifndef AV_ALIGN_BUFZ
#define AV_ALIGN_BUFZ(size, align) (((size) + align - 1U))
#endif

#ifndef AV_ALIGN
#define AV_ALIGN(addr, align) ((void *)(((uint32_t)(addr) + align - 1U) & (~(uint32_t)(align - 1U))))
#endif

#ifdef __cplusplus
}
#endif

#endif /* __COMMON_H__ */

