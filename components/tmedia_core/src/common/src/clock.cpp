/*
 * Copyright (C) 2023 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <time.h>
#include <tmedia_core/common/clock.h>
#include <tmedia_core/util/util_inc.h>
#include <tmedia_core/common/syslog.h>

TMClock::TMClock()
{
    mTime.time_base = 0;
    mTime.timestamp = 0;
    mTime.valid = false;
    mTimeDefault.time_base = TM_TIME_BASE_DEFAULT;
    mTimeDefault.timestamp = 0;
    mTimeDefault.valid = false;
}

TMClock::~TMClock()
{
}

int TMClock::Set(TMClock_t &time)
{
    if (time.time_base > 0)
    {
        mTime = time;
    }
    else
    {
        return TMResult::TM_EINVAL;
    }

    mTime.valid = true;
    return TMResult::TM_OK;
}

int TMClock::Set(TMClockType_e type)
{
    if (type != TMCLOCK_MONOTONIC && type != TMCLOCK_REALTIME)
    {
        LOG_E("%s %d err type:%d\n", __func__, __LINE__, type);
        return TMResult::TM_EINVAL;
    }

    mTime.timestamp = GetTime(type);
    mTime.time_base = TM_TIME_BASE_DEFAULT;
    mTime.valid = true;

    return TMResult::TM_OK;
}

int TMClock::Set(uint64_t timestamp, uint32_t time_base)
{
    if (time_base > 0)
    {
        mTime.timestamp = timestamp;
        mTime.time_base = time_base;
    }
    else
    {
        return TMResult::TM_EINVAL;
    }

    mTime.valid = true;
    return TMResult::TM_OK;
}

int TMClock::Set(uint32_t dst_time_base, TMClock_t src_time)
{
    int ret = TMResult::TM_OK;
    if (dst_time_base > 0 && src_time.time_base > 0)
    {
        mTime.time_base = dst_time_base;
        ret = TimestampConvert(&mTime, &src_time);
        if (ret != TMResult::TM_OK)
        {
            return ret;
        }
    }
    else
    {
        return TMResult::TM_EINVAL;
    }

    return ret;
}

int TMClock::SetDefault(TMClock_t &time_default)
{
    if (time_default.time_base > 0)
    {
        mTimeDefault = time_default;
    }
    else
    {
        return TMResult::TM_EINVAL;
    }
    mTimeDefault.valid = true;
    return TMResult::TM_OK;
}

int TMClock::SetDefault(TMClockType_e type)
{
    if (type != TMCLOCK_MONOTONIC && type != TMCLOCK_REALTIME)
    {
        LOG_E("%s %d err type:%d\n", __func__, __LINE__, type);
        return TMResult::TM_EINVAL;
    }

    mTimeDefault.timestamp = GetTime(type);
    mTimeDefault.time_base = TM_TIME_BASE_DEFAULT;
    mTimeDefault.valid = true;

    return TMResult::TM_OK;
}

int TMClock::SetDefault(uint64_t timestamp, uint32_t time_base)
{
    if (time_base > 0)
    {
        mTimeDefault.timestamp = timestamp;
        mTimeDefault.time_base = time_base;
    }
    else
    {
        return TMResult::TM_EINVAL;
    }

    mTimeDefault.valid = true;
    return TMResult::TM_OK;
}

int TMClock::SetDefault(uint32_t dst_time_base, TMClock_t src_time)
{
    int ret = TMResult::TM_OK;
    if (dst_time_base > 0 && src_time.time_base > 0)
    {
        mTimeDefault.time_base = dst_time_base;
        ret = TimestampConvert(&mTimeDefault, &src_time);
        if (ret != TMResult::TM_OK)
        {
            return ret;
        }
    }
    else
    {
        return TMResult::TM_EINVAL;
    }

    return ret;
}

TMClock_t TMClock::Get(void)
{
    return mTime;
}

TMClock_t TMClock::GetDefault(void)
{
    return mTimeDefault;
}

uint64_t TMClock::GetTime(TMClockType_e type)
{
    timespec time;
    if (type == TMCLOCK_MONOTONIC)
    {
        clock_gettime(CLOCK_MONOTONIC, &time);
        return (time.tv_sec * 1000000 + time.tv_nsec / 1000);
    }
    else if (type == TMCLOCK_REALTIME)
    {
        clock_gettime(CLOCK_REALTIME, &time);
        return (time.tv_sec * 1000000 + time.tv_nsec / 1000);
    }
    else
    {
        LOG_E("error clock type\n");
    }
    return 0;
}

int TMClock::TimestampConvert(TMClock_t *dst, TMClock_t *src)
{
    if (!src || !dst)
    {
        return TMResult::TM_EINVAL;
    }
    if (src->time_base == 0 || dst->time_base == 0)
    {
        return TMResult::TM_EINVAL;
    }

    dst->timestamp = ((double)src->timestamp / src->time_base) * dst->time_base;
    dst->valid = true;

    return TMResult::TM_OK;
}