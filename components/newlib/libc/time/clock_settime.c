/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <errno.h>
#include <sys/time.h>
#include <time.h>
#include <aos/kernel.h>

/*
POSIX.1-2001, POSIX.1-2008, SUSv2.

On POSIX systems on which these functions are available, the symbol
_POSIX_TIMERS is defined in <unistd.h> to a value greater than 0.
The symbols _POSIX_MONOTONIC_CLOCK, _POSIX_CPUTIME,
_POSIX_THREAD_CPUTIME indicate that CLOCK_MONOTONIC,
CLOCK_PROCESS_CPUTIME_ID, CLOCK_THREAD_CPUTIME_ID are available.
(See also sysconf(3).)
*/
__attribute__((weak)) int clock_settime(clockid_t clockid, const struct timespec *tp)
{
    uint64_t time_ms = 0;

    /* only CLOCK_REALTIME can be set */
    if ((clockid != CLOCK_REALTIME) || (tp == NULL) ||
        (tp->tv_nsec < 0) || (tp->tv_nsec >= 1000000000UL)) {
        errno = EINVAL;
        return -1;
    }

    time_ms = (tp->tv_sec * 1000) + (tp->tv_nsec / 1000000);
    aos_calendar_time_set(time_ms);

    return 0;
}
