/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <errno.h>
#include <sys/time.h>
#include <time.h>
#include <aos/kernel.h>

extern struct timespec g_basetime;
extern struct timespec last_readtime;

/*
POSIX.1-2001, POSIX.1-2008, SUSv2.

On POSIX systems on which these functions are available, the symbol
_POSIX_TIMERS is defined in <unistd.h> to a value greater than 0.
The symbols _POSIX_MONOTONIC_CLOCK, _POSIX_CPUTIME,
_POSIX_THREAD_CPUTIME indicate that CLOCK_MONOTONIC,
CLOCK_PROCESS_CPUTIME_ID, CLOCK_THREAD_CPUTIME_ID are available.
(See also sysconf(3).)
*/
__attribute__((weak)) int clock_gettime(clockid_t clockid, struct timespec *tp)
{
    uint64_t time_ms = 0;

    if (tp == NULL) {
        errno = EINVAL;
        return -1;
    }

    if (clockid == CLOCK_MONOTONIC) {
        time_ms = aos_now_ms();
        tp->tv_sec = time_ms / 1000;
        tp->tv_nsec = (time_ms % 1000) * 1000000;
    } else if (clockid == CLOCK_REALTIME) {
        time_ms = aos_calendar_time_get();
        tp->tv_sec = time_ms / 1000;
        tp->tv_nsec = (time_ms % 1000) * 1000000;
    } else {
        errno = EINVAL;
        return -1;
    }

    return 0;
}

int clock_gettime_adjust(void)
{
    int ret;
    struct timespec ts_begin, ts_end;
    ret = clock_gettime(CLOCK_MONOTONIC, &ts_begin);
    if (ret != 0) {
        return -1;
    }

    ret = clock_gettime(CLOCK_MONOTONIC, &ts_end);
    if (ret != 0) {
        return -1;
    }

    long long adjust_ns =
        (1LL * ts_end.tv_sec * 1000000000 + ts_end.tv_nsec) -
        (1LL * ts_begin.tv_sec * 1000000000 + ts_begin.tv_nsec);

    return adjust_ns;
}
