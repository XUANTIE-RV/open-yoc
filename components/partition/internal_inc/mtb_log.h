/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#ifndef __MTB_LOG_H__
#define __MTB_LOG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>

#undef LOG_TAG
#define LOG_TAG "[mtb]"

#ifdef CONFIG_DEBUG
#if CONFIG_DEBUG == 3
#define MTB_LOGD(format, ...) printf(LOG_TAG"[D] ""[%s, %d]"format"\r\n", __func__, __LINE__, ##__VA_ARGS__)
#define MTB_LOGI(format, ...) printf(LOG_TAG"[I] ""[%s, %d]"format"\r\n", __func__, __LINE__, ##__VA_ARGS__)
#define MTB_LOGW(format, ...) printf(LOG_TAG"[W] ""[%s, %d]"format"\r\n", __func__, __LINE__, ##__VA_ARGS__)
#define MTB_LOGE(format, ...) printf(LOG_TAG"[E] ""[%s, %d]"format"\r\n", __func__, __LINE__, ##__VA_ARGS__)
#elif CONFIG_DEBUG == 2
#define MTB_LOGD(format, ...)
#define MTB_LOGI(format, ...) printf(LOG_TAG"[I] "format"\r\n",##__VA_ARGS__)
#define MTB_LOGW(format, ...)
#define MTB_LOGE(format, ...) printf(LOG_TAG"[E] "format"\r\n",##__VA_ARGS__)
#elif CONFIG_DEBUG == 1
#define MTB_LOGD(format, ...)
#define MTB_LOGI(format, ...)
#define MTB_LOGW(format, ...)
#define MTB_LOGE(format, ...) printf(LOG_TAG"[E] "format"\r\n",##__VA_ARGS__)
#else
#define MTB_LOGD(format, ...)
#define MTB_LOGI(format, ...)
#define MTB_LOGW(format, ...)
#define MTB_LOGE(format, ...)
#endif
#else
#define MTB_LOGD(format, ...)
#define MTB_LOGI(format, ...)
#define MTB_LOGW(format, ...)
#define MTB_LOGE(format, ...)
#endif

#ifdef __cplusplus
}
#endif

#endif /* UPDATE_LOG_H_ */
