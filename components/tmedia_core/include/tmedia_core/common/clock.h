/*
 * Copyright (C) 2023 Alibaba Group Holding Limited
 */

#ifndef TM_CLOCK_H
#define TM_CLOCK_H

#include <stdint.h>

#define TM_TIME_BASE_DEFAULT 1000000  //Hz

typedef enum
{
    TMCLOCK_MONOTONIC = 0,            //relative time since the system was powered on, unit us
    TMCLOCK_REALTIME                  //wall time since 1970-01-01, unit us
} TMClockType_e;

typedef struct
{
    uint64_t timestamp;               //time value in time_base
    uint32_t time_base;               //unit, Hz
    bool valid;
} TMClock_t;

class TMClock
{

public:
    TMClock();
    virtual ~TMClock();
    int Set(TMClock_t &time);
    int SetDefault(TMClock_t &time_default);
    int Set(TMClockType_e type);
    int SetDefault(TMClockType_e type);
    TMClock_t Get(void);
    TMClock_t GetDefault(void);

    static uint64_t GetTime(TMClockType_e type);
    static int TimestampConvert(TMClock_t *dst, TMClock_t *src);
private:
    TMClock_t mTime;                //take the timebase of each entity as the unit
    TMClock_t mTimeDefault;         //take TM_TIME_BASE_DEFAULT as the unit
};

#endif