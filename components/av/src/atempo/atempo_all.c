/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#include "av/atempo/atempo.h"
#include "av/atempo/atempo_all.h"

/**
 * @brief  regist audio atempoer for sonic
 * @return 0/-1
 */
int atempo_register_sonic()
{
    extern struct atempo_ops atempo_ops_sonic;
    return atempo_ops_register(&atempo_ops_sonic);
}

/**
 * @brief  regist audio atempoer for ipc
 * @return 0/-1
 */
int atempo_register_ipc()
{
    extern struct atempo_ops atempo_ops_ipc;
    return atempo_ops_register(&atempo_ops_ipc);
}

/**
 * @brief  regist audio atempoer for dummy
 * @return 0/-1
 */
int atempo_register_dummy()
{
    extern struct atempo_ops atempo_ops_dummy;
    return atempo_ops_register(&atempo_ops_dummy);
}



