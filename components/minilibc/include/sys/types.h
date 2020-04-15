/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef __INCLUDE_SYS_TYPES_H
#define __INCLUDE_SYS_TYPES_H

#ifndef _SYS_CDEFS_H
#define _SYS_CDEFS_H    1
# include <features.h>
#endif   /* sys/cdefs.h */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define OK              0
typedef int             pid_t;
typedef uint32_t        clock_t;

#ifdef __cplusplus
}
#endif

#endif

