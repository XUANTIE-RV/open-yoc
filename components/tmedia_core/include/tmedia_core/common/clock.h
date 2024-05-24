/*
 * Copyright (C) 2023 Alibaba Group Holding Limited
 */

#ifndef TM_CLOCK_H
#define TM_CLOCK_H

#include <stdint.h>

#define TM_TIME_BASE_DEFAULT 1000000 // Hz

typedef enum
{
    TMCLOCK_MONOTONIC = 0, // relative time since the system was powered on, unit us
    TMCLOCK_REALTIME       // wall time since 1970-01-01, unit us
} TMClockType_e;

typedef struct
{
    uint64_t timestamp; // time value in time_base
    uint32_t time_base; // unit, Hz
    bool valid;
} TMClock_t;

class TMClock
{

  public:
    TMClock();
    virtual ~TMClock();

    /* set time from external*/
    int Set(TMClock_t &time);
    /*set time from clock_gettime, MONOTONIC or REALTIME */
    int Set(TMClockType_e type);
    /* set time from external*/
    int Set(uint64_t timestamp, uint32_t time_base);
    /*set time from different time base, auto run TimestampConvert */
    int Set(uint32_t dst_time_base, TMClock_t src_time);

    int SetDefault(TMClock_t &time_default);
    int SetDefault(TMClockType_e type);
    int SetDefault(uint64_t timestamp, uint32_t time_base);
    int SetDefault(uint32_t dst_time_base, TMClock_t src_time);
    TMClock_t Get(void);
    TMClock_t GetDefault(void);

    /**
     * @return  >0 success, 0 failure.
     */
    static uint64_t GetTime(TMClockType_e type);
    /**
     * @return  TMResult::TM_OK success, else failure.
     */
    static int TimestampConvert(TMClock_t *dst, TMClock_t *src);

  private:
    TMClock_t mTime;        // take the timebase of each entity as the unit
    TMClock_t mTimeDefault; // take TM_TIME_BASE_DEFAULT as the unit
};

#endif