/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     secure.c
 * @brief    source file for setting system secure.c.
 * @version  V1.0
 * @date     11. Aug 2020
 * @vendor   csky
 * @chip     pangu
 ******************************************************************************/
#include <stdint.h>
#include <soc.h>
#include "wj_tipc_ll.h"
#include "drv/porting.h"
#include "sys_tipc.h"

void soc_tipc_enable_secure(soc_tipc_module_t ip)
{
    wj_tipc_regs_t *tipc_base = (wj_tipc_regs_t *)WJ_TIPC_BASE;
    wj_tipc_enable_secure(tipc_base, (uint32_t)ip);
}

void soc_tipc_disable_secure(soc_tipc_module_t ip)
{
    wj_tipc_regs_t *tipc_base = (wj_tipc_regs_t *)WJ_TIPC_BASE;
    wj_tipc_disable_secure(tipc_base, (uint32_t)ip);
}