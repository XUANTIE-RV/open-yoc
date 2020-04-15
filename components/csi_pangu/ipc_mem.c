/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
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

static ipc_mem_t g_ipc_mem;

#define IPC_MEM_START (0xFFE0000)
#ifdef CONFIG_CHIP_PANGU_CPU0
    #define IPC_MEM_ADDR (IPC_MEM_START)
    #define IPC_MEM_SIZE (16 * 1024)
    #define IPC_MEM_BLOCK_SIZE (IPC_MEM_SIZE/2)
#endif

#ifdef CONFIG_CHIP_PANGU_CPU1
    #define IPC_MEM_ADDR (IPC_MEM_START + 0x4000)
    #define IPC_MEM_SIZE (8 * 1024)
    #define IPC_MEM_BLOCK_SIZE (IPC_MEM_SIZE/1)
#endif

#ifdef CONFIG_CHIP_PANGU_CPU2
    #define IPC_MEM_ADDR (IPC_MEM_START + 0x6000)
    #define IPC_MEM_SIZE (8 * 1024)
    #define IPC_MEM_BLOCK_SIZE (IPC_MEM_SIZE/1)
#endif

void drv_ipc_mem_init(void)
{
    if (g_ipc_mem.inited == 1) {
        return;
    }

    g_ipc_mem.addr          = IPC_MEM_ADDR;
    g_ipc_mem.size          = IPC_MEM_SIZE;
    g_ipc_mem.block_size    = IPC_MEM_BLOCK_SIZE;
    g_ipc_mem.used_flag     = 0;
    g_ipc_mem.inited        = 1;

}

void *drv_ipc_mem_alloc(int *len)
{
    int cnt = g_ipc_mem.size / g_ipc_mem.block_size;

    for (int i = 0; i < cnt; i++) {
        if ((g_ipc_mem.used_flag & (1 << i)) == 0) {
            g_ipc_mem.used_flag |= (1 << i);

            *len = g_ipc_mem.block_size;
            return ((char *)g_ipc_mem.addr + i * g_ipc_mem.block_size);
        }
    }

    *len = 0;
    return NULL;
}

void drv_ipc_mem_free(void *p)
{
    int addr = (int)p;
    int offset = addr - g_ipc_mem.addr;

    if (offset % g_ipc_mem.block_size == 0) {
        int i = offset / g_ipc_mem.block_size;

        g_ipc_mem.used_flag &= ~(1 << i);
    }

}

int drv_ipc_mem_use_cache(void)
{
    return 0;
}