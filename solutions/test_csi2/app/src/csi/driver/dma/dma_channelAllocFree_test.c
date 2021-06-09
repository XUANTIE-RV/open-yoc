/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <dma_test.h>

int test_dma_channelAllocFree(char *args)
{
    csi_dma_ch_t ch_hd;
    csi_error_t ret_sta;
    test_dma_args_t td;
    int ret;
    uint32_t get_data[2];

    ret = args_parsing(args, get_data, 2);
    TEST_CASE_ASSERT_QUIT(ret == 0, "num_of_parameter error");

    td.dma_idx = (uint8_t)get_data[0];
    td.channel_idx = (uint8_t)get_data[1];

    TEST_CASE_TIPS("DMA idx is %d", td.dma_idx);
    TEST_CASE_TIPS("Channel is %d", td.channel_idx);

    ret_sta = csi_dma_ch_alloc(&ch_hd, td.channel_idx, td.dma_idx);
    TEST_CASE_ASSERT_QUIT(ret_sta == CSI_OK, "DMA %d alloc channel %d error,should return CSI_OK,but returned %d.", td.dma_idx, td.channel_idx, ret_sta);

    ret_sta = csi_dma_ch_alloc(&ch_hd, td.channel_idx, td.dma_idx);
    TEST_CASE_ASSERT_QUIT(ret_sta != CSI_OK, "DMA %d channel %d alloc twice error,shouldn't return CSI_OK,but returned %d.", td.dma_idx, td.channel_idx, ret_sta);

    csi_dma_ch_free(&ch_hd);

    ret_sta = csi_dma_ch_alloc(&ch_hd, td.channel_idx, td.dma_idx);
    TEST_CASE_ASSERT_QUIT(ret_sta == CSI_OK, "DMA %d alloc channel %d error,should return CSI_OK,but returned %d.", td.dma_idx, td.channel_idx, ret_sta);

    csi_dma_ch_free(&ch_hd);
    csi_dma_ch_free(&ch_hd);

    return 0;
}
