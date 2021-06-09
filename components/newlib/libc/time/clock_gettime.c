/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
// #include <soc.h>
#include <sys/time.h>
#include <time.h>

extern struct timespec g_basetime;
extern struct timespec last_readtime;
extern int coretimspec(struct timespec *ts);

#ifdef CONFIG_CSI_V2
extern uint64_t csi_tick_get_us(void);
int clock_gettime(clockid_t clockid, struct timespec *tp)
{
    int ret = 0;

    if (clockid == CLOCK_MONOTONIC) {

        uint64_t now_time = csi_tick_get_us();
        uint64_t div_1000000 = 1000000U, div_1000 = 1000U, temp;

        temp = now_time / div_1000000;
        tp->tv_sec = (time_t)(temp);

        temp = (now_time % div_1000000) * div_1000;
        tp->tv_nsec = (long)(temp);

    } else {

        ret = -1;

    }

    return ret;
}
#else
int clock_gettime(clockid_t clockid, struct timespec *tp)
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
#endif
