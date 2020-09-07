/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#include "aef/eq.h"
#include "aef/eq_all.h"

/**
 * @brief  regist equalizer for silan
 * @return 0/-1
 */
int eqx_register_silan()
{
    extern struct eqx_ops eqx_ops_silan;
    return eqx_ops_register(&eqx_ops_silan);
}

/**
 * @brief  regist equalizer for ipc
 * @return 0/-1
 */
int eqx_register_ipc()
{
    extern struct eqx_ops eqx_ops_ipc;
    return eqx_ops_register(&eqx_ops_ipc);
}



