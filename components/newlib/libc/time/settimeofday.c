/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <yoc_config.h>
#include <sys/time.h>
#include <time.h>

extern int clock_settime(clockid_t clockid, const struct timespec *tp);

int settimeofday(const struct timeval *tv, const struct timezone *tz)
{
    struct timespec ts;

    if (!tv || tv->tv_usec >= USEC_PER_SEC) {
        return -1;
    }

    /* Convert the timeval to a timespec */

    ts.tv_sec  = tv->tv_sec;
    ts.tv_nsec = tv->tv_usec * NSEC_PER_USEC;

    /* Let clock_settime do the work */

    return clock_settime(CLOCK_REALTIME, &ts);
}

