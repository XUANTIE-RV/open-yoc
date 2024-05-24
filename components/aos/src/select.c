/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
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
