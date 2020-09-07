/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#include "mca/mca.h"
#include "mca/mca_all.h"

/**
 * @brief  regist mca for local
 * @return 0/-1
 */
int mcax_register_local()
{
    extern struct mcax_ops mcax_ops_local;
    return mcax_ops_register(&mcax_ops_local);
}

/**
 * @brief  regist mca for ipc
 * @return 0/-1
 */
int mcax_register_ipc()
{
    extern struct mcax_ops mcax_ops_ipc;
    return mcax_ops_register(&mcax_ops_ipc);
}


