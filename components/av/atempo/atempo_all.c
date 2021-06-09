/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#include "atempo/atempo.h"
#include "atempo/atempo_all.h"

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



