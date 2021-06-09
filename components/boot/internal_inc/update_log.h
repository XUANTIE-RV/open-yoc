/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#ifndef UPDATE_LOG_H_
#define UPDATE_LOG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>

#undef LOG_TAG
#define LOG_TAG "[boot]"

#ifdef CONFIG_DEBUG
#if CONFIG_DEBUG == 3
#define UPD_LOGD(format, ...) printf(LOG_TAG"[D] "format"\r\n",##__VA_ARGS__)
#define UPD_LOGI(format, ...) printf(LOG_TAG"[I] "format"\r\n",##__VA_ARGS__)
#define UPD_LOGW(format, ...) printf(LOG_TAG"[W] "format"\r\n",##__VA_ARGS__)
#define UPD_LOGE(format, ...) printf(LOG_TAG"[E] "format"\r\n",##__VA_ARGS__)
#elif CONFIG_DEBUG == 2
#define UPD_LOGD(format, ...)
#define UPD_LOGI(format, ...) printf(LOG_TAG"[I] "format"\r\n",##__VA_ARGS__)
#define UPD_LOGW(format, ...)
#define UPD_LOGE(format, ...) printf(LOG_TAG"[E] "format"\r\n",##__VA_ARGS__)
#elif CONFIG_DEBUG == 1
#define UPD_LOGD(format, ...)
#define UPD_LOGI(format, ...)
#define UPD_LOGW(format, ...)
#define UPD_LOGE(format, ...) printf(LOG_TAG"[E] "format"\r\n",##__VA_ARGS__)
#else
#define UPD_LOGD(format, ...)
#define UPD_LOGI(format, ...)
#define UPD_LOGW(format, ...)
#define UPD_LOGE(format, ...)
#endif
#else
#define UPD_LOGD(format, ...)
#define UPD_LOGI(format, ...)
#define UPD_LOGW(format, ...)
#define UPD_LOGE(format, ...)
#endif

#ifdef __cplusplus
}
#endif

#endif /* UPDATE_LOG_H_ */
