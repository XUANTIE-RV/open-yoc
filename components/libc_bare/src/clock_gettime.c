 /*
 * Copyright (C) 2017-2024 Alibaba Group Holding Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/******************************************************************************
 * @file     clock_gettime.c
 * @brief    clock_gettime()
 * @version  V1.0
 * @date     08. May 2019
 ******************************************************************************/
#include <time.h>
#include <errno.h>
#include <drv/tick.h>

int clock_gettime(clockid_t clock_id, struct timespec *tp)
{
    uint64_t time_ms = 0;

    if (tp == NULL) {
        errno = EINVAL;
        return -1;
    }

    if (clock_id == CLOCK_MONOTONIC) {
        time_ms = csi_tick_get_ms();
        tp->tv_sec = time_ms / 1000;
        tp->tv_nsec = (time_ms % 1000) * 1000000;
    } else if (clock_id == CLOCK_REALTIME) {
        // FIXME:
        errno = ENOSYS;
        return -1;
    } else {
        errno = EINVAL;
        return -1;
    }

    return 0;
}
