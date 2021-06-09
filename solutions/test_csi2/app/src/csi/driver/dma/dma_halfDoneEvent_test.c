/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <dma_test.h>

static volatile uint8_t lock1 = 0;
static volatile uint8_t lock2 = 0;
static csi_dma_ch_t ch_hd;

static void dma_ch_callback(csi_dma_ch_t *dma_ch, csi_dma_event_t event, void *arg)
{
    if (event == DMA_EVENT_TRANSFER_HALF_DONE) {
        lock1 = 0;
    }

    if (event == DMA_EVENT_TRANSFER_DONE) {
        lock2 = 0;
    }
}


static int data_init(void *addr_src, void *addr_dest, uint32_t data_length)
{
    uint32_t i;

    for (i = 0; i < data_length; i++) {
        *((uint8_t *)addr_src + i) = i;
        *((uint8_t *)addr_dest + i) = 0;
    }

    return 0;
}


int test_dma_halfDoneEvent(char *args)
{
    csi_error_t ret_sta;
    void *src, *dest;
    int ret;
    test_dma_args_t td;

    uint32_t get_data[3];

    ret = args_parsing(args, get_data, 3);
    TEST_CASE_ASSERT_QUIT(ret == 0, "num_of_parameter error");

    td.dma_idx = (uint8_t)get_data[0];
    td.channel_idx = (uint8_t)get_data[1];
    td.transfer_length = (uint32_t)get_data[2];

    TEST_CASE_TIPS("DMA idx is %d", td.dma_idx);
    TEST_CASE_TIPS("Channel is %d", td.channel_idx);
    TEST_CASE_TIPS("Transfer length is %d", td.transfer_length);

    src = (void *)malloc(td.transfer_length);
    dest = (void *)malloc(td.transfer_length);

    if ((src == NULL) || (dest == NULL)) {
        free(src);
        free(dest);
        TEST_CASE_WARN_QUIT("malloc error");
    }

    ret = data_init(src, dest, td.transfer_length);

    if (ret != 0) {
        free(src);
        free(dest);
        TEST_CASE_WARN_QUIT("data init error");
    }

    ret_sta = csi_dma_ch_alloc(&ch_hd, td.channel_idx, td.dma_idx);

    if (ret_sta != 0) {
        free(src);
        free(dest);
        TEST_CASE_ASSERT_QUIT(ret_sta == CSI_OK, "DMA %d alloc channel %d error,should return CSI_OK,but returned %d.", td.dma_idx, td.channel_idx, ret_sta);
    }

    csi_dma_ch_config_t config;
    config.src_inc = DMA_ADDR_INC;
    config.dst_inc = DMA_ADDR_INC;
    config.src_tw = DMA_DATA_WIDTH_8_BITS;
    config.dst_tw = DMA_DATA_WIDTH_8_BITS;
    config.trans_dir = DMA_MEM2MEM;
    config.group_len = 8;
    config.half_int_en  = 1;

    ret_sta = csi_dma_ch_config(&ch_hd, &config);

    if (ret_sta != 0) {
        free(src);
        free(dest);
        csi_dma_ch_free(&ch_hd);
        TEST_CASE_ASSERT_QUIT(ret_sta == CSI_OK, "DMA %d channel %d config error,should return CSI_OK,but returned %d.", td.dma_idx, td.channel_idx, ret_sta);
    }

    ret_sta = csi_dma_ch_attach_callback(&ch_hd, dma_ch_callback, NULL);

    if (ret_sta != 0) {
        free(src);
        free(dest);
        csi_dma_ch_free(&ch_hd);
        TEST_CASE_ASSERT_QUIT(ret_sta == CSI_OK, "DMA %d channel %d attach callback error,should return CSI_OK,but returned %d.", td.dma_idx, td.channel_idx, ret_sta);
    }

    lock1 = 1;
    lock2 = 1;

    csi_dma_ch_start(&ch_hd, src, dest, td.transfer_length);

    while (lock1);

    uint32_t r_counter = 0, i = 0;

    for (i = 0; i < td.transfer_length; i++) {
        if ((*(uint8_t *)src) == (*(uint8_t *)dest)) {
            r_counter++;
        }
    }

    while (lock2);

    csi_dma_ch_stop(&ch_hd);
    csi_dma_ch_detach_callback(&ch_hd);
    csi_dma_ch_free(&ch_hd);
    free(src);
    free(dest);
    return 0;
}
