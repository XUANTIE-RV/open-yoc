/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef __DMA_TEST__
#define __DMA_TEST__

#include <stdint.h>
#include <drv/dma.h>
#include <soc.h>
#include <string.h>
#include <stdlib.h>
#include <autotest.h>
#include <test_log.h>
#include <test_config.h>
#include <test_common.h>

typedef struct {
    uint8_t     dma_idx;
    uint8_t     channel_idx;
    uint8_t     data_width;
    uint8_t     src_addr_inc;
    uint8_t     dest_addr_inc;
    uint8_t     group_length;
    uint32_t    transfer_length;
} test_dma_args_t;



extern int test_dma_interface(char *args);
extern int test_dma_MemoryToMemory(char *args);
extern int test_dma_channelAllocFree(char *args);
extern int test_dma_halfDoneEvent(char *args);
extern int test_dma_main(char *args);
#endif
