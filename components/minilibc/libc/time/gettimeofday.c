/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <sys/time.h>
#include <time.h>

extern int clock_gettime(clockid_t clockid, struct timespec *tp);

/*
SVr4, 4.3BSD.  POSIX.1-2001 describes gettimeofday() but not
settimeofday().  POSIX.1-2008 marks gettimeofday() as obsolete,
recommending the use of clock_gettime(2) instead.
*/
int gettimeofday(struct timeval *tv, struct timezone *tz)
{
    struct timespec ts;
    int             ret;

    if (!tv) {
        return -1;
    }

    ret = clock_gettime(CLOCK_REALTIME, &ts);

    if (ret == 0) {
        tv->tv_sec = ts.tv_sec;
        tv->tv_usec = ts.tv_nsec / NSEC_PER_USEC;
    }

    return ret;
}
