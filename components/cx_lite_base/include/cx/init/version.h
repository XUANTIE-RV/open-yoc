/**
 * @file version.h
 * @copyright Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef CX_VERSION_H
#define CX_VERSION_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef CONFIG_CX_PREVIEW_EDITION
#define CX_VERSION_POSTFIX  -preview
#else
#define CX_VERSION_POSTFIX  -release
#endif

#ifndef CONFIG_CX_VERSION
#define CONFIG_CX_VERSION "v1.0.x"
#endif

#define CX_STR(str) #str
#define CX_STR2(str) CX_STR(str)
#define CX_VERSION CONFIG_CX_VERSION CX_STR2(CX_VERSION_POSTFIX)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* CX_VERSION_H */

