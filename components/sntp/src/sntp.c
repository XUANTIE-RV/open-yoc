// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "sntp.h"
#include <stdlib.h>
#include <unistd.h>
#include "lwip/apps/sntp.h"
#include "aos/log.h"

#ifndef CONFIG_NTP_CTS_ZONE
#define CTS_ZONE 8
#else
#define CTS_ZONE CONFIG_NTP_CTS_ZONE
#endif

static const char *TAG = "sntp";

/// SNTP time update mode
typedef enum {
    SNTP_SYNC_MODE_IMMED   /*!< Smooth time updating. Time error is gradually reduced using adjtime function. If the difference between SNTP response time and system time is large (more than 35 minutes) then update immediately. */
} sntp_sync_mode_t;

/// SNTP sync status
typedef enum {
    SNTP_SYNC_STATUS_RESET,         // Reset status.
    SNTP_SYNC_STATUS_COMPLETED,     // Time is synchronized.
    SNTP_SYNC_STATUS_IN_PROGRESS,   // Smooth time sync in progress.
} sntp_sync_status_t;

static sntp_op_mode_t g_op_mode = SNTP_CLIENT_UNICAST;
static sntp_sync_time_cb_t time_sync_notification_cb = NULL;
static int server_idx = 0;

void __attribute__((weak)) sntp_set_system_time(int64_t sec, int64_t us)
{
    struct timeval tv;

    tv.tv_sec = sec;
    tv.tv_usec = us;
    LOGD(TAG, "set time:%lld ms", ((int64_t)tv.tv_sec * 1000000L + (int64_t)tv.tv_usec) / 1000);
    settimeofday(&tv, NULL);
    if (time_sync_notification_cb) {
        time_sync_notification_cb(&tv);
    }
}

// set a callback function for time synchronization notification
void sntp_set_time_sync_notification_cb(sntp_sync_time_cb_t callback)
{
    time_sync_notification_cb = callback;
}

void sntp_add_server(char *server)
{
    sntp_setservername(server_idx++, server);
}

void sntp_set_op_mode(sntp_op_mode_t op_mode)
{
    g_op_mode = op_mode;
}

void sntp_sync_start()
{
    if (server_idx == 0) {
        //1.cn.pool.ntp.org is more reliable
#ifdef SNTP_SERVER_ADDRESS
        sntp_add_server(SNTP_SERVER_ADDRESS);
#else
        sntp_add_server("ntp1.aliyun.com");
#endif
    }

    if (g_op_mode == SNTP_CLIENT_UNICAST) {
        sntp_setoperatingmode(SNTP_OPMODE_POLL);
    } else if (g_op_mode == SNTP_CLIENT_BROADCAST) {
        sntp_setoperatingmode(SNTP_OPMODE_LISTENONLY);
    } else {
        LOGE(TAG, "sntp op mode error");
        return;
    }

    sntp_init();
}

void sntp_sync_stop(void)
{
    sntp_stop();
}