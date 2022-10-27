/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <sys/time.h>
#include <time.h>

extern struct timespec g_basetime;
extern struct timespec last_readtime;
extern int coretimspec(struct timespec *ts);

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
    struct timespec ts;
    uint32_t        carry;
    int             ret = 0;

    tp->tv_sec = 0;

    if (clockid == CLOCK_MONOTONIC) {
        ret = coretimspec(tp);
        if(ret < 0) {
            return -1;
        }
    }

    if (clockid == CLOCK_REALTIME) {
        ret = coretimspec(&ts);

        if (ret == 0) {

            if (ts.tv_nsec < last_readtime.tv_nsec) {
                ts.tv_nsec += NSEC_PER_SEC;
                ts.tv_sec -= 1;
            }

            carry = (ts.tv_nsec - last_readtime.tv_nsec) + g_basetime.tv_nsec;

            if (carry >= NSEC_PER_SEC) {
                carry -= NSEC_PER_SEC;
                tp->tv_sec += 1;
            }

            tp->tv_sec += (ts.tv_sec - last_readtime.tv_sec) + g_basetime.tv_sec;
            tp->tv_nsec = carry;
        }
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
