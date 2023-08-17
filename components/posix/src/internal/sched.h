/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */

#ifndef __POSIX_INTERNAL_SCHED_H
#define __POSIX_INTERNAL_SCHED_H

#include <sched.h>
#include <pthread.h>
#include <aos/kernel.h>

/* Convert the schedule policy of posix to aos. */
static inline int sched_policy_posix2aos(int policy)
{
    switch (policy) {
        case SCHED_OTHER:
            return AOS_KSCHED_OTHER;
        case SCHED_FIFO:
            return AOS_KSCHED_FIFO;
        case SCHED_RR:
            return AOS_KSCHED_RR;
        default:
            return -1;
    }
}

/* Convert the schedule policy of aos to posix. */
static inline int sched_policy_aos2posix(int policy)
{
    switch (policy) {
        case AOS_KSCHED_OTHER:
            return SCHED_OTHER;
        case AOS_KSCHED_FIFO:
            return SCHED_FIFO;
        case AOS_KSCHED_RR:
            return SCHED_RR;
        default:
            return -1;
    }
}

/* In aos: lower priority value means higher priority.
 * In posix standard: higher priority value means higher priority.
 */
static inline int sched_priority_posix2aos(int policy, int priority)
{
    int pri = aos_sched_get_priority_max(policy) - priority;
    return pri < 0 ? 0 : pri;
}

#define sched_priority_aos2posix sched_priority_posix2aos

#endif /*__POSIX_INTERNAL_SCHED_H*/
