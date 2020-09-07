/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef AOS_LOG_H
#define AOS_LOG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>

typedef enum {
    AOS_LL_NONE = 0,  /* disable log */
    AOS_LL_FATAL,     /* fatal log will output */
    AOS_LL_ERROR,     /* fatal + error log will output */
    AOS_LL_WARN,      /* fatal + warn + error log will output(default level) */
    AOS_LL_INFO,      /* info + warn + error log will output */
    AOS_LL_DEBUG,     /* debug + info + warn + error + fatal log will output */
    LOG_MAX
} aos_log_level_t;

#ifndef CONFIG_LOGLEVEL_DEFAULT

#if defined(CONFIG_DEBUG) && CONFIG_DEBUG
#define CONFIG_LOGLEVEL_DEFAULT  5
#else
#define CONFIG_LOGLEVEL_DEFAULT  4
#endif

#endif

/**
 * Get the log level.
 * 
 * @return current log level, defined in type aos_log_level_t
 */
int aos_get_log_level(void);

/**
 * Set the log level.
 *
 * @param[in]  log_level  level to be set,must be one of AOS_LL_NONE,AOS_LL_FATAL,AOS_LL_ERROR,AOS_LL_WARN,AOS_LL_INFO or AOS_LL_DEBUG.
 */
void aos_set_log_level(aos_log_level_t log_level);


/**
 * Dump buffer by hex.
 * @param[in]  tag  tag
 * @param[in]  buffer buffer
 * @param[in]  len buffer len
 * @returns zero on success.  On error, yoc_err is returned
 */
int aos_log_hexdump(const char* tag, char *buffer, int len);

/**
 * use log function
 * @param[in]  tag  tag
 * @param[in]  priority priority
 * @param[in]  fmt
 * @returns zero on success.  On error, yoc_err is returned
 */
int aos_log_tag(const char *tag, int log_level, const char *fmt, ...);

#if CONFIG_LOGLEVEL_DEFAULT >= 1
#define LOGF(tag, format, ...)      aos_log_tag(tag, AOS_LL_FATAL, format"\n",##__VA_ARGS__)
#else
#define LOGF(tag, format, ...)
#endif

#if CONFIG_LOGLEVEL_DEFAULT >= 2
#define LOGE(tag, format, ...)      aos_log_tag(tag, AOS_LL_ERROR, format"\n",##__VA_ARGS__)
#else
#define LOGE(tag, format, ...)
#endif

#if CONFIG_LOGLEVEL_DEFAULT >= 3
#define LOGW(tag, format, ...)      aos_log_tag(tag, AOS_LL_WARN, format"\n",##__VA_ARGS__)
#else
#define LOGW(tag, format, ...)
#endif

#if CONFIG_LOGLEVEL_DEFAULT >= 4
#define LOGI(tag, format, ...)      aos_log_tag(tag, AOS_LL_INFO, format"\n",##__VA_ARGS__)
#else
#define LOGI(tag, format, ...)
#endif

#if CONFIG_LOGLEVEL_DEFAULT >= 5
#define LOGD(tag, format, ...)      aos_log_tag(tag, AOS_LL_DEBUG, format"\n",##__VA_ARGS__)
#else
#define LOGD(tag, format, ...)
#endif

/* None Level Control */
#define LOGV(tag, format, ...)     aos_log_tag(tag, AOS_LL_NONE, format,##__VA_ARGS__)

#define LOG_HEXDUMP(tag, buffer, len)    aos_log_hexdump(tag, buffer, len)

#ifdef __cplusplus
}
#endif

#endif /* AOS_LOG_H */
