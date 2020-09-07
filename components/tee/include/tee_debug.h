/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef TEE_DEBUG_H
#define TEE_DEBUG_H
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define LOG_NONE  0
#define LOG_ERROR 1
#define LOG_INFO  2
#define LOG_DEBUG 3
#define LOG_MAX   4

#ifdef CONFIG_TEE_DEBUG
#ifndef CONFIG_DEBUG_LEVLE
#define CONFIG_DEBUG_LEVLE LOG_ERROR
#endif

#if CONFIG_DEBUG_LEVLE > LOG_NONE
#define TEE_LOG(format, ...) printf(format, ##__VA_ARGS__)
#else
#define TEE_LOG(format, ...)
#endif

#if CONFIG_DEBUG_LEVLE >= LOG_DEBUG
#define TEE_LOGD(format, ...)    TEE_LOG("[D]"format, ##__VA_ARGS__)
static inline void hex_dump(const char *msg, const uint8_t *buf, uint32_t len)
{
    int hex_i;
    hex_i = (len);
    printf("%s", msg);

    while (hex_i) {
        if (((len) - hex_i--) % 16 == 0) {
            printf("\n[0x%08x]", buf + (len) - hex_i - 1);
        }

        printf("0x%02x ", (buf)[(len) - hex_i - 1]);
    }

    printf("\n");
}
#define TEE_HEX_DUMP(msg,buf,len)  hex_dump(msg, buf, len)
#else
#define TEE_LOGD(format, ...)
#define TEE_HEX_DUMP(msg,data,len)
#endif

#if CONFIG_DEBUG_LEVLE >= LOG_INFO
#define TEE_LOGI(format, ...)    TEE_LOG("[I]"format, ##__VA_ARGS__)
#else
#define TEE_LOGI(format, ...)
#endif

#if CONFIG_DEBUG_LEVLE >= LOG_ERROR
#define TEE_LOGE(format, ...)    TEE_LOG("[E]"format, ##__VA_ARGS__)
#else
#define TEE_LOGE(format, ...)
#endif
#else
#define TEE_HEX_DUMP(msg,data,len)
#define TEE_LOG(format, ...)
#define TEE_LOGD(format, ...)
#define TEE_LOGI(format, ...)
#define TEE_LOGE(format, ...)
#endif
#endif

