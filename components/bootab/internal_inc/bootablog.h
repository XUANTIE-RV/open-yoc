/*
* Copyright (C) 2015-2017 Alibaba Group Holding Limited
*/
#ifndef __BOOTABLOG_H__
#define __BOOTABLOG_H__

#ifdef AOS_COMP_ULOG
#include <ulog/ulog.h>

#define TAG "[BOOTAB]"
#define BABLOGE(fmt, args...)  LOGE(TAG, fmt, ##args)

#define BABLOGI(fmt, args...)  LOGI(TAG, fmt, ##args)

#define BABLOGD(fmt, args...)  LOGD(TAG, fmt, ##args)

#define BABLOGW(fmt, args...)  LOGW(TAG, fmt, ##args)

#else

#include <stdio.h>

#define LOG_TAG "[BOOTAB]"
#ifdef CONFIG_DEBUG
#if CONFIG_DEBUG == 3
#define BABLOGD(format, ...) printf(LOG_TAG"[D] " "[%s:%d]" format "\r\n", __func__, __LINE__, ##__VA_ARGS__)
#define BABLOGI(format, ...) printf(LOG_TAG"[I] " "[%s:%d]" format "\r\n", __func__, __LINE__, ##__VA_ARGS__)
#define BABLOGW(format, ...) printf(LOG_TAG"[W] " "[%s:%d]" format "\r\n", __func__, __LINE__, ##__VA_ARGS__)
#define BABLOGE(format, ...) printf(LOG_TAG"[E] " "[%s:%d]" format "\r\n", __func__, __LINE__, ##__VA_ARGS__)
#elif CONFIG_DEBUG == 2
#define BABLOGD(format, ...)
#define BABLOGI(format, ...) printf(LOG_TAG"[I] " "[%s:%d]" format "\r\n", __func__, __LINE__, ##__VA_ARGS__)
#define BABLOGW(format, ...)
#define BABLOGE(format, ...) printf(LOG_TAG"[E] " "[%s:%d]" format "\r\n", __func__, __LINE__, ##__VA_ARGS__)
#elif CONFIG_DEBUG == 1
#define BABLOGD(format, ...)
#define BABLOGI(format, ...)
#define BABLOGW(format, ...)
#define BABLOGE(format, ...) printf(LOG_TAG"[E] " "[%s:%d]" format "\r\n", __func__, __LINE__, ##__VA_ARGS__)
#else
#define BABLOGD(format, ...)
#define BABLOGI(format, ...)
#define BABLOGW(format, ...)
#define BABLOGE(format, ...)
#endif
#else
#define BABLOGD(format, ...)
#define BABLOGI(format, ...)
#define BABLOGW(format, ...)
#define BABLOGE(format, ...)
#endif

#endif /*AOS_COMP_ULOG*/

#endif
