/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <codec_test.h>

int test_codec_pauseResume(char *args)
{
    csi_error_t ret_sta;
    csi_codec_t hd;
    csi_codec_output_config_t ch_config;
    csi_codec_output_t ch;
    test_codec_args_t td;
    int ret;
    ringbuffer_t output_ring_buffer;


    uint32_t get_data[8];

    ret = args_parsing(args, get_data, 8);
    TEST_CASE_ASSERT_QUIT(ret == 0, "num_of_parameter error");

    td.codec_idx = (uint32_t)get_data[0];
    td.ch_idx = (uint32_t)get_data[1];
    td.sample_rate = (uint32_t)get_data[2];
    td.bit_width = (uint32_t)get_data[3];
    td.sound_channel_num = (uint32_t)get_data[4];
    td.data_size = (uint32_t)get_data[5];
    td.buffer_size = (uint32_t)get_data[6];
    td.mode = (uint32_t)get_data[7];

    TEST_CASE_TIPS("test codec idx is %d", td.codec_idx);
    TEST_CASE_TIPS("test codec channel is %d", td.ch_idx);
    TEST_CASE_TIPS("config sample rate is %d Hz", td.sample_rate);
    TEST_CASE_TIPS("config converter bit width is %d", td.bit_width);
    TEST_CASE_TIPS("config number of soundtrack per channel is %d", td.sound_channel_num);
    TEST_CASE_TIPS("config output data size is %d bytes", td.data_size);
    TEST_CASE_TIPS("config buffer size is %d", td.buffer_size);
    TEST_CASE_TIPS("config mode is %d", td.mode);


    uint8_t *cache_buffer = NULL;
    cache_buffer = malloc(td.buffer_size);

    if (cache_buffer == NULL) {
        TEST_CASE_WARN_QUIT("malloc error");
    }

    ret_sta = csi_codec_init(&hd, td.codec_idx);

    if (ret_sta != 0) {
        free(cache_buffer);
        TEST_CASE_ASSERT_QUIT(1 == 0, "codec %d init error,should return CSI_OK,but returned %d", td.codec_idx, ret_sta);
    }

    ch_config.sample_rate = td.sample_rate;
    ch_config.bit_width = td.bit_width;
    ch_config.buffer = cache_buffer;
    ch_config.buffer_size = td.buffer_size;
    ch_config.sound_channel_num = td.sound_channel_num;
    ch_config.period = 512;
    ch_config.mode = td.mode;

    ch.ring_buf = &output_ring_buffer;
    ret_sta = csi_codec_output_config(&ch, &ch_config);

    if (ret_sta != 0) {
        csi_codec_uninit(&hd);
        free(cache_buffer);
        TEST_CASE_ASSERT_QUIT(1 == 0, "output config error,should return CSI_OK,but returned %d", ret_sta);
    }


    ret_sta = csi_codec_output_open(&hd, &ch, td.ch_idx);

    if (ret_sta != 0) {
        csi_codec_uninit(&hd);
        free(cache_buffer);
        TEST_CASE_ASSERT_QUIT(1 == 0, "output open error,should return CSI_OK,but returned %d", ret_sta);
    }



    csi_dma_ch_t dma_ch;

    ret_sta = csi_codec_output_link_dma(&ch, &dma_ch);

    if (ret_sta != 0) {
        csi_codec_output_close(&ch);
        csi_codec_uninit(&hd);
        free(cache_buffer);
        TEST_CASE_ASSERT_QUIT(1 == 0, "output link DMA error,should return CSI_OK,but returned %d", ret_sta);
    }


    ret_sta = csi_codec_output_start(&ch);

    if (ret_sta != 0) {
        csi_codec_output_close(&ch);
        csi_codec_uninit(&hd);
        free(cache_buffer);
        TEST_CASE_ASSERT_QUIT(1 == 0, "output start error,should return CSI_OK,but returned %d", ret_sta);

    }


    uint32_t ret_num;

    ret_num = csi_codec_output_write(&ch, (void *)0x04, td.data_size);
    TEST_CASE_ASSERT(ret_num == td.data_size, "output write error,should return %d,but returned %d", td.data_size, ret_num);

    ret_sta = csi_codec_output_pause(&ch);
    TEST_CASE_ASSERT(ret_sta == 0, "output pause error,should return CSI_OK,but returned %d", ret_sta);

    tst_mdelay(10);

    uint32_t free_size1 = 0, free_size2 = 0;
    free_size1 = csi_codec_output_buffer_avail(&ch);

    tst_mdelay(10);
    free_size2 = csi_codec_output_buffer_avail(&ch);

    if ((free_size1 != free_size2) || (free_size2 == td.buffer_size)) {
        TEST_CASE_ASSERT(1 == 0, "get available buffer size error");
    }

    ret_sta = csi_codec_output_resume(&ch);

    tst_mdelay(100);

    free_size1 = csi_codec_output_buffer_avail(&ch);

    if (free_size1 == free_size2) {
        TEST_CASE_ASSERT(1 == 0, "output resume error", td.ch_idx);
    }

    csi_codec_output_stop(&ch);

    ret_sta = csi_codec_output_link_dma(&ch, NULL);
    TEST_CASE_ASSERT(ret_sta == 0, "output unlink dma error");

    csi_codec_output_close(&ch);

    csi_codec_uninit(&hd);
    free(cache_buffer);

    return 0;
}
