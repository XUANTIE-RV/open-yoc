/*
 * Copyright (C) 2019-2021 Alibaba Group Holding Limited
 */

#ifndef __LOG_H__
#define __LOG_H__


#ifdef __cplusplus
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
extern "C" {
#else
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif

#if 0
#define ATTR_UNUSED __attribute__((unused))
#else
#define ATTR_UNUSED
#endif

enum LogLevel {
    LogLevel_VERBOSE ATTR_UNUSED = 8,
    LogLevel_DEBUG ATTR_UNUSED = 7,
    LogLevel_INFO ATTR_UNUSED = 6,
    LogLevel_WARNING ATTR_UNUSED = 4,
    LogLevel_ERR ATTR_UNUSED = 3,
    LogLevel_CRIT ATTR_UNUSED = 2,
    LogLevel_ALERT ATTR_UNUSED = 1
};

#ifdef _WIN32
#define LOG_NEWLINE "\r\n"
#else
#define LOG_NEWLINE "\n"
#endif

#ifdef HAASUI_LOG_TO_ULOG
extern int ulog(const unsigned char s, const char *mod, const char *f,
         const unsigned long l, const char *fmt, ...);
#define greenui_printLog(LEVEL, fmt, ...) \
    ulog(LEVEL, "HaaS_UI", __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#elif defined(__linux__) && defined(HAASUI_LOG_TO_SYSLOG)
#include <syslog.h>
#ifdef _JQUICK_BUILD_RELEASE
#define greenui_printLog(LEVEL, fmt, ...) syslog(7, fmt, ##__VA_ARGS__)
#else
#define greenui_printLog(LEVEL, fmt, ...)           \
        do {                                        \
            syslog(7, fmt, ##__VA_ARGS__);          \
            printf(fmt LOG_NEWLINE, ##__VA_ARGS__); \
        } while(0)
#define greenui_printLog_no_newline(LEVEL, fmt, ...) syslog(7, fmt, ##__VA_ARGS__)
#endif
#else
#define greenui_printLog(LEVEL, fmt, ...) printf(fmt LOG_NEWLINE, ##__VA_ARGS__)
#define greenui_printLog_no_newline(LEVEL, fmt, ...) printf(fmt, ##__VA_ARGS__)
#endif

#ifndef greenui_printLog_no_newline
#define greenui_printLog_no_newline greenui_printLog
#endif

#ifdef LOGE
#undef LOGE
#endif

#define LOGE(fmt, ...)                           \
    greenui_printLog(LogLevel_ERR, fmt, ##__VA_ARGS__)

#ifdef LOGD
#undef LOGD
#endif
#ifdef __DEBUG
#define LOGD(fmt, ...) greenui_printLog(LogLevel_DEBUG, fmt, ##__VA_ARGS__)
#else
#define LOGD(fmt, ...)
#endif

#ifdef LOGI
#undef LOGI
#endif

#define LOGI(fmt, ...) greenui_printLog(LogLevel_INFO, fmt, ##__VA_ARGS__)

#ifndef LOG_FATAL_IF
#define LOG_FATAL_IF(cond, fmt, ...)                 \
    if (cond) {                                      \
        greenui_printLog(LogLevel_ERR, fmt, ##__VA_ARGS__);\
    }
#endif

#ifndef LOG_ALWAYS_FATAL
#define LOG_ALWAYS_FATAL(fmt, ...) greenui_printLog(LogLevel_ERR, fmt, ##__VA_ARGS__)
#endif

#ifdef __cplusplus
}
#endif

#endif /* __LOG_H__ */