/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#include "avutil/av_errno.h"

static int _av_errno;

/**
 * @brief  set the global errno for av, ugly for special use(xiaoya, etc)
 * @param  [in] errno
 * @return
 */
void av_errno_set(int errno)
{
    _av_errno = errno;
}

/**
 * @brief  get the global av errno on play errno.
 * attention: this av-errno is not accurate, just for statistic-online
 * @return AV_ERRNO_XXX
 */
int av_errno_get()
{
    return _av_errno;
}


