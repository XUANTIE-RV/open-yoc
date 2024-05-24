/*
 * Copyright (C) 2023 Alibaba Group Holding Limited
 */

#include <string.h>
#include <unistd.h>
#include <signal.h>

#include <tmedia_core/common/common_inc.h>
#include <tmedia_core/util/util_timer.h>

using namespace std;

TMUtilTimer::TMUtilTimer(TMTimerHandler timerHandler, uint32_t intervalUSec,
                         void *usrArg, bool runImmediately)
{
    mTimerHandler = timerHandler;
    mIntervalUSec = intervalUSec;
    mThreadRunFlag = false;
    mUsrArg = usrArg;
    mRunImmediately = runImmediately;
    mTimerID = (timer_t)0;
}

TMUtilTimer::~TMUtilTimer()
{
    Stop();
}

void TMUtilTimer::TMUtilTimerThread(union sigval val)
{
    TMUtilTimer *inst = (TMUtilTimer *)(val.sival_ptr);
    if (inst->mTimerHandler != NULL && inst->mThreadRunFlag == true)
    {
        inst->mTimerHandler(inst->mUsrArg);
    }
}

int TMUtilTimer::Start()
{
#ifdef __linux__
    int ret;

    struct sigevent sev;
    memset(&sev, 0, sizeof(sev));
    struct itimerspec timer_spec;

    // Create timer
    sev.sigev_notify = SIGEV_THREAD;
    sev.sigev_notify_function = TMUtilTimerThread;
    sev.sigev_value.sival_ptr = this;
    ret = timer_create(CLOCK_MONOTONIC, &sev, &mTimerID);
    if (ret != 0)
    {
        LOG_E("timer_create() failed, errno: %d, %s\n", errno, strerror(errno));
        return TMResult::TM_EINVAL;
    }
    mThreadRunFlag = true;

    // Set timer interval
    timer_spec.it_interval.tv_sec = mIntervalUSec / 1000000;
    timer_spec.it_interval.tv_nsec = mIntervalUSec % 1000000 * 1000;

    if (mRunImmediately)
    {
        timer_spec.it_value.tv_sec = 0;
        timer_spec.it_value.tv_nsec = 1;
    }
    else
    {
        timer_spec.it_value.tv_sec = timer_spec.it_interval.tv_sec;
        timer_spec.it_value.tv_nsec = timer_spec.it_interval.tv_nsec;
    }
    ret = timer_settime(mTimerID, 0, &timer_spec, NULL);
    if (ret != 0)
    {
        LOG_E("timer_settime() failed, errno: %d, %s\n", errno, strerror(errno));
        mThreadRunFlag = false;
        return TMResult::TM_EINVAL;
    }

    return TMResult::TM_OK;
#else
    return TMResult::TM_NOT_SUPPORT;
#endif
}

int TMUtilTimer::Stop()
{
#ifdef __linux__
    int ret;
    mThreadRunFlag = false;

    if (mTimerID == NULL)
        return TMResult::TM_OK;

    // stop timer
    struct itimerspec timer_spec;
    timer_spec.it_interval.tv_sec = 0;
    timer_spec.it_interval.tv_nsec = 0;
    timer_spec.it_value.tv_sec = 0;
    timer_spec.it_value.tv_nsec = 0;
    ret = timer_settime(mTimerID, 0, &timer_spec, NULL);
    if (ret != 0)
    {
        LOG_E("timer_settime() failed, errno: %d, %s\n", errno, strerror(errno));;
        return TMResult::TM_EINVAL;
    }

    // delete timer
    ret = timer_delete(mTimerID);
    if (ret != 0)
    {
        LOG_E("timer_delete() failed, errno: %d, %s\n", errno, strerror(errno));
        return TMResult::TM_EINVAL;
    }

    mTimerID = NULL;
    return TMResult::TM_OK;
#else
    return TMResult::TM_NOT_SUPPORT;
#endif
}

