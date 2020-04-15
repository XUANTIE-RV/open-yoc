/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */


/******************************************************************************
 * @file     ipc_mem.c
 * @brief    source file for the ipc memroy
 * @version  V1.0
 * @date     06. Mar 2019
 * @vendor   csky
 * @chip     pangu
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <csi_config.h>

typedef struct {
    int             addr;
    size_t          size;
    int             block_size;
    int32_t         used_flag;
    int             inited;
} ipc_mem_t;

//static ipc_mem_t g_ipc_mem;


void drv_ipc_mem_init(void)
{
    ;
}

void *drv_ipc_mem_alloc(int *len)
{

    *len = 0;
    return NULL;
}

void drv_ipc_mem_free(void *p)
{
    ;
}

int drv_ipc_mem_use_cache(void)
{
    return 0;
}
