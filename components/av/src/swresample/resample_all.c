/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#include "av/swresample/resample.h"
#include "av/swresample/resample_all.h"

/**
 * @brief  regist swresample for ipc
 * @return 0/-1
 */
int resample_register_ipc()
{
    extern struct resx_ops resx_ops_ipc;
    return resx_ops_register(&resx_ops_ipc);
}

/**
 * @brief  regist swresample for speex
 * @return 0/-1
 */
int resample_register_speex()
{
    extern struct resx_ops resx_ops_speex;
    return resx_ops_register(&resx_ops_speex);
}

/**
 * @brief  regist swresample for dummy
 * @return 0/-1
 */
int resample_register_dummy()
{
    extern struct resx_ops resx_ops_dummy;
    return resx_ops_register(&resx_ops_dummy);
}
