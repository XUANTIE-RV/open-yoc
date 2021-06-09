/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <dma_test.h>


test_func_map_t dma_test_funcs_map[] = {
    {"DMA_INTERFACE", test_dma_interface},
    {"DMA_MEMORY_TO_MEMORY", test_dma_MemoryToMemory},
    {"DMA_CHANNEL_ALLOC_FREE", test_dma_channelAllocFree},
    {"DMA_HALF_DONE_EVENT", test_dma_halfDoneEvent},
};

int test_dma_main(char *args)
{
    uint8_t i;

    for (i = 0; i < sizeof(dma_test_funcs_map) / sizeof(test_func_map_t); i++) {
        if (!strcmp((void *)_mc_name, dma_test_funcs_map[i].test_func_name)) {
            (*(dma_test_funcs_map[i].test_func_p))(args);
            return 0;
        }
    }

    TEST_CASE_TIPS("DMA module don't support this command.");
    return -1;
}
