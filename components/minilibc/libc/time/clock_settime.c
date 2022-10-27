/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <sys/time.h>
#include <time.h>
#include <csi_core.h>

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
__attribute__((weak)) int clock_settime(clockid_t clockid, const struct timespec *tp)
{
    struct timespec bias;
    uint32_t      flags;
    int             ret = 0;

    if (clockid == CLOCK_REALTIME) {
        /* Interrupts are disabled here so that the in-memory time
         * representation and the RTC setting will be as close as
         * possible.
         */

        flags = csi_irq_save();

        /* Get the elapsed time since power up (in milliseconds).  This is a
         * bias value that we need to use to correct the base time.
         */

        coretimspec(&bias);
        g_basetime.tv_sec = tp->tv_sec;
        g_basetime.tv_nsec = tp->tv_nsec;
        last_readtime.tv_sec = bias.tv_sec;
        last_readtime.tv_nsec = bias.tv_nsec;
        csi_irq_restore(flags);

    } else {
        ret = -1;
    }

    return ret;
}
