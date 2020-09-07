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

#ifndef __SNTP_H__
#define __SNTP_H__

#include <time.h>
#include <sys/time.h>

#ifdef __cplusplus
extern "C"
{
#endif

/// SNTP operating modes
typedef enum {
    SNTP_CLIENT_UNICAST,
    SNTP_CLIENT_BROADCAST
} sntp_op_mode_t;

/**
 * @brief SNTP callback function for notifying about time sync event
 *
 * @param tv Time received from SNTP server.
 */
typedef void (*sntp_sync_time_cb_t) (struct timeval *tv);

/**
 * @brief sntp sync start
 *  start sntp sync when call this function,default op_mode is SNTP_CLIENT_UNICAST
 *  use "ntp1.aliyun.com" as default server if sntp_add_server not called before
 */
void sntp_sync_start();


/**
 * @brief add sntp server
 * @param server    The name or IP of SNTP server, use "ntp1.aliyun.com" as default server if NULL
 *
 */
void sntp_add_server(char *server);

/**
 * @brief sntp set operating modes
 * call this before `sntp_sync_start`
 *
 * @param op_mode   The operating modes, see `sntp_op_mode_t`
 *
 */
void sntp_set_op_mode(sntp_op_mode_t op_mode);


/**
 * @brief Set a callback function for time synchronization notification
 *
 * @param callback a callback function
 */
void sntp_set_time_sync_notification_cb(sntp_sync_time_cb_t callback);

/**
 * @brief sntp sync stop
 *
 */
void sntp_sync_stop(void);

#ifdef __cplusplus
}
#endif
#endif // __SNTP_H__
