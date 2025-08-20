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

#include <aos/kernel.h>
#include <sys/select.h>
//#include "lwip/opt.h"
//#include "lwip/sockets.h"

__attribute__((weak)) int select2(int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset,
                                  struct timeval *timeout, void *semaphore)
{
    uint32_t tomeout_ms;

#if defined(CONFIG_AOS_LWIP)
    extern int lwip_select2(int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset, struct timeval *timeout, void *semaphore);
#if defined(AOS_COMP_VFS) && AOS_COMP_VFS
    extern int aos_vfs_fd_offset_get(void);
    if (maxfdp1 < aos_vfs_fd_offset_get() + 1)
#endif
        return lwip_select2(maxfdp1, readset, writeset, exceptset, timeout, semaphore);
#elif defined(CONFIG_SAL)
    extern int sal_select2(int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset, struct timeval *timeout, void *semaphore);
#if defined(AOS_COMP_VFS) && AOS_COMP_VFS
    extern int aos_vfs_fd_offset_get(void);
    if (maxfdp1 < aos_vfs_fd_offset_get() + 1)
#endif
        return sal_select2(maxfdp1, readset, writeset, exceptset, timeout, semaphore);
#endif

    if (timeout) {
        tomeout_ms = timeout->tv_sec * 1000 + timeout->tv_usec / 1000;
    } else {
        tomeout_ms = AOS_WAIT_FOREVER;
    }

    if (semaphore && aos_sem_is_valid(semaphore)) {
        aos_sem_wait(semaphore, tomeout_ms);
    } else {
        aos_msleep(tomeout_ms);
    }

    return  0;
}


/* dummy select, fix compile error with no net evn */
__attribute__((weak)) int select(int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset,
                                 struct timeval *timeout)
{
    return select2(maxfdp1, readset, writeset, exceptset, timeout, NULL);
}
