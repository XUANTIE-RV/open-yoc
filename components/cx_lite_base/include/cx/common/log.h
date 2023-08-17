/**
 * @file log.h
 * @copyright Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef CX_COMMON_LOG_H
#define CX_COMMON_LOG_H

#include <ulog/ulog.h>


#ifdef CONFIG_CX_DEBUG
#define CX_LOGD LOGD
#define CX_LOGI LOGI
#define CX_LOGW LOGW
#define CX_LOGE LOGE
#else
#define CX_LOGD
#define CX_LOGI
#define CX_LOGW LOGW
#define CX_LOGE LOGE
#endif

#endif /* CX_COMMON_LOG_H */

