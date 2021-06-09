/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Lmiited
 */
#ifndef _APP_MAIN_H_
#define _APP_MAIN_H_

#include <stdio.h>
#include "sec_crypto_rng.h"
#include "ulog/ulog.h"

#ifdef CONFIG_SECURITY_DEMO_KEY_MGR
#include "key_mgr.h"
#endif

#define TAG "secure_demo"
#define TRACE_ERR(e) LOGD(TAG, "===err:%s, %d, %x\n", __FUNCTION__, __LINE__, e)

#define TRACE_STR(s) LOGD(TAG, "%s, %d, %s\n", __FUNCTION__, __LINE__, s)
#define TRACE() LOGD(TAG, "%s, %d\n", __FUNCTION__, __LINE__)

#define CHECK_RET_WITHOUT_RET_P(x, ret) \
    do                                  \
    {                                   \
        if (!(x))                       \
        {                               \
            TRACE_ERR(ret);             \
            return;                     \
        }                               \
    } while (0)

#define CHECK_RET_WITHOUT_RET(x) \
    do                           \
    {                            \
        if (!(x))                \
        {                        \
            TRACE_ERR(-1);       \
            return;              \
        }                        \
    } while (0)

#define CHECK_RESULT(x)       \
    do                        \
    {                         \
        if (!(x))             \
        {                     \
            TRACE_ERR(-1);    \
            return;           \
        }                     \
        else                  \
        {                     \
                              \
            TRACE_STR("passed!"); \
        }                     \
    } while (0)

/* wifi & net */
typedef enum
{
    MODE_WIFI_TEST = -2,
    MODE_WIFI_CLOSE = -1,
    MODE_WIFI_NORMAL = 0,
    MODE_WIFI_PAIRING = 1
} wifi_mode_e;
wifi_mode_e app_network_init(void);
int wifi_connecting();
void wifi_lpm_enable(int lpm_en);
void wifi_set_lpm(int lpm_on);
int wifi_internet_is_connected();
int wifi_getmac(uint8_t mac[6]);
/* event id define */
#define EVENT_NTP_RETRY_TIMER (EVENT_USER + 1)
#define EVENT_NET_CHECK_TIMER (EVENT_USER + 2)
#define EVENT_NET_NTP_SUCCESS (EVENT_USER + 3)
#define EVENT_NET_LPM_RECONNECT (EVENT_USER + 4)

/* user define event */
#define EVENT_DATA_REPORT (EVENT_USER + 1)
#define EVENT_CHANNEL_CHECK (EVENT_USER + 2)
#define EVENT_CHANNEL_CLOSE (EVENT_USER + 3)

#define ALIMQTT_CMD_NET 1
#define ALIMQTT_CMD_PUSH 2

void sec_crypto_demo_main();
void mbedtls_demo_main();
void key_mgr_demo_main();
int sec_crypto_ecc_demo();
void sec_alimqtt_demo_main();
void aes_test_main(void);
void misc_demo_main();

#endif