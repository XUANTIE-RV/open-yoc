/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#include "output/ao.h"
#include "output/output.h"
#include "output/output_all.h"

/**
 * @brief  regist audio output for alsa
 * @return 0/-1
 */
int ao_register_alsa()
{
    extern struct ao_ops ao_ops_alsa;
    return ao_ops_register(&ao_ops_alsa);
}

/**
 * @brief  regist audio output for ipc
 * @return 0/-1
 */
int ao_register_ipc()
{
    extern struct ao_ops ao_ops_ipc;
    return ao_ops_register(&ao_ops_ipc);
}



