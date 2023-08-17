/*
 * Copyright (C) 2023 Alibaba Group Holding Limited
 */

#ifndef TM_UTIL_TIMER_H
#define TM_UTIL_TIMER_H

#include <string>
#include <tmedia_core/common/common_inc.h>

using namespace std;

typedef int (*TMTimerHandler)(void *arg);

class TMUtilTimer
{
public:
    TMUtilTimer(TMTimerHandler timerHandler, uint32_t intervalUSec,
                void *usrArg = NULL, bool runImmediately = false);
    ~TMUtilTimer();

    static void TMUtilTimerThread(union sigval val);
    int Start();
    int Stop();
    //Pause();
    //Resume();

private:
    TMTimerHandler mTimerHandler;
    uint32_t mIntervalUSec;
    void *mUsrArg;
    bool mRunImmediately;

    struct timespec mInitTime;
    pthread_t mThreadID;
    bool mThreadRunFlag;
    timer_t mTimerID;
};

#endif  /* TM_UTIL_TIMER_H */
