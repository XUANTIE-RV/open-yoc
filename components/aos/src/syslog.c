/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     lib_syslog.c
 * @brief    CSI Source File for syslog
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <sys/time.h>
#include <aos/log.h>


const char g_yoc_log_string[LOG_MAX] = {
    'U',          /*LOG_NONE*/
    'F',          /*LOG_FATAL*/
    'E',          /*LOG_ERROR*/
    'W',          /*LOG_WARN*/
    'I',          /*LOG_INFO*/
    'D'           /*LOG_DEBUG*/
};

int g_syslog_level = LOG_MAX;

void aos_set_log_level(aos_log_level_t log_level) {
    g_syslog_level = log_level;
}

int aos_get_log_level(void) {
    return g_syslog_level;
}

#if 0
int aos_log_tagv(const char *tag, int log_level, const char *fmt, va_list args)
{
    struct timespec ts;
    int ret;

    if (log_level > g_syslog_level ) {
        return -1;
    }

    if (tag != NULL) {
        /* Get the current time */
        ret = clock_gettime(CLOCK_MONOTONIC, &ts);

        if (ret == 0) {
            printf("[%6d.%06d][%c][%-8s]", ts.tv_sec, (int)ts.tv_nsec / 1000, g_yoc_log_string[log_level], tag);
        } else {
            printf("[%c][%-8s]", g_yoc_log_string[log_level], tag);
        }
    }

    return vprintf(fmt, args);
}
#endif

__attribute__((weak)) int aos_log_tag(const char *tag, int log_level, const char *fmt, ...)
{
    struct timespec ts;
    int ret;
    va_list args;

    if (log_level > g_syslog_level ) {
        return -1;
    }

    if (tag != NULL) {
        /* Get the current time */
        ret = clock_gettime(CLOCK_MONOTONIC, &ts);

        if (ret == 0) {
             printf("[%6d.%06d][%c][%-8s]", ts.tv_sec, (int)ts.tv_nsec / 1000, g_yoc_log_string[log_level], tag);
        } else {
            printf("[%c][%-8s]", g_yoc_log_string[log_level], tag);
        }
    }

    /* printf args */
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);

    return 0;
}

int aos_log_hexdump(const char *tag, char *buffer, int len)
{
    struct timespec ts;
    int ret, i;

    /* Get the current time */
    ret = clock_gettime(CLOCK_MONOTONIC, &ts);
    ret = -1;

    if (ret == 0) {
        // printf("[%6d.%06d][%s]\n", ts.tv_sec, (int)ts.tv_nsec / 1000, tag);
    } else {
        printf("[%s]\n", tag);
    }

    printf("0x0000: ");

    for (i = 0; i < len; i++) {
        printf("0x%02x ", buffer[i]);

        if (i % 8 == 7) {
            printf("\n");
            printf("0x%04x: ", i + 1);
        }
    }

    printf("\n");
    return 0;
}
