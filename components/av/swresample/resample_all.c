/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#include "swresample/resample.h"
#include "swresample/resample_all.h"

/**
 * @brief  regist swresample for silan
 * @return 0/-1
 */
int resample_register_silan()
{
    extern struct resx_ops resx_ops_silan;
    return resx_ops_register(&resx_ops_silan);
}

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
