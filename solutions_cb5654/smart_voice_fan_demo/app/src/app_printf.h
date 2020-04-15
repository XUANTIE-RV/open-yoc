/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef _APP_PRINTF_H
#define _APP_PRINTF_H

#ifdef __cplusplus
extern "C" {
#endif

int printf(const char *format,...);

typedef enum {
    LL_NONE = 0,  /* disable log */
    LL_ERROR,     /* fatal + error log will output */
    LL_INFO,      /* info + warn + error log will output */
    LL_DEBUG,     /* debug + info + warn + error + fatal log will output */
    MAX
} print_level_t;

#ifndef CONFIG_LOG_LEVEL_DEFAULT
#ifdef CONFIG_DEBUG
#define CONFIG_LEVEL_DEFAULT  3
#else
#define CONFIG_LEVEL_DEFAULT  2
#endif
#endif

#if CONFIG_LOG_LEVEL_DEFAULT >= 1
#define LOGE(format, ...)      printf(format, ##__VA_ARGS__)
#else
#define LOGE(format, ...)
#endif

#if CONFIG_LOG_LEVEL_DEFAULT >= 2
#define LOGI(format, ...)      printf(format, ##__VA_ARGS__)
#else
#define LOGI(format, ...)
#endif

#if CONFIG_LOG_LEVEL_DEFAULT >= 3
#define LOGD(format, ...)      printf(format, ##__VA_ARGS__)
#else
#define LOGD(format, ...)
#endif

#ifdef __cplusplus
}
#endif

#endif /* _APP_PRINTF_H */
