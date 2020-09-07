/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#include "aef/aef.h"
#include "aef/aef_all.h"

/**
 * @brief  regist audio effecter for sona
 * @return 0/-1
 */
int aefx_register_sona()
{
    extern struct aefx_ops aefx_ops_sona;
    return aefx_ops_register(&aefx_ops_sona);
}

/**
 * @brief  regist audio effecter for ipc
 * @return 0/-1
 */
int aefx_register_ipc()
{
    extern struct aefx_ops aefx_ops_ipc;
    return aefx_ops_register(&aefx_ops_ipc);
}



