/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <codec_test.h>

int test_codec_outputBuffer(char *args)
{
    csi_error_t ret_sta;
    csi_codec_t hd;
    csi_codec_output_config_t ch_config;
    csi_codec_output_t ch;
    test_codec_args_t td;
    int ret;
    ringbuffer_t output_ring_buffer;

    uint32_t get_data[3];

    ret = args_parsing(args, get_data, 3);
    TEST_CASE_ASSERT_QUIT(ret == 0, "num_of_parameter error");

    td.codec_idx = (uint32_t)get_data[0];
    td.ch_idx = (uint32_t)get_data[1];
    td.buffer_size = (uint32_t)get_data[2];

    TEST_CASE_TIPS("test codec idx is %d", td.codec_idx);
    TEST_CASE_TIPS("test codec channel is %d", td.ch_idx);
    TEST_CASE_TIPS("config buffer size is %d", td.buffer_size);

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

    ch.ring_buf = &output_ring_buffer;
    ret_sta = csi_codec_output_open(&hd, &ch, td.ch_idx);

    if (ret_sta != 0) {
        csi_codec_uninit(&hd);
        free(cache_buffer);
        TEST_CASE_ASSERT_QUIT(1 == 0, "output open error,should return CSI_OK,but returned %d", ret_sta);
    }

    ch_config.buffer = cache_buffer;
    ch_config.buffer_size = td.buffer_size;
    ch_config.bit_width = 8;
    ch_config.mode = CODEC_OUTPUT_DIFFERENCE;
    ch_config.period = 512;
    ch_config.sample_rate = 48000;

    ret_sta = csi_codec_output_config(&ch, &ch_config);

    if (ret_sta != 0) {
        csi_codec_output_close(&ch);
        csi_codec_uninit(&hd);
        free(cache_buffer);
        TEST_CASE_ASSERT_QUIT(1 == 0, "output config error,should return CSI_OK,but returned %d", ret_sta);
    }

    uint32_t ret_num;

    ret_sta = csi_codec_output_buffer_reset(&ch);
    TEST_CASE_ASSERT(ret_sta == 0, "output buffer reset error,should return CSI_OK,but returned %d", ret_sta);

    ret_num = csi_codec_output_buffer_avail(&ch);
    TEST_CASE_ASSERT(ret_num == td.buffer_size, "freeing cache size is not equal to that of getting availed");

    ret_num = csi_codec_output_write(&ch, (void *)0x4, td.buffer_size / 4);
    TEST_CASE_ASSERT(ret_num == td.buffer_size / 4, "output write error,should return %d,but returned %d", td.ch_idx, td.buffer_size / 4, ret_num);

    ret_num = csi_codec_output_buffer_avail(&ch);
    TEST_CASE_ASSERT(ret_num == td.buffer_size, "the size of freeing cache is not equal to that of getting availed");

    ret_num = csi_codec_output_write_async(&ch, (void *)0x4, td.buffer_size);
    TEST_CASE_ASSERT(ret_num == td.buffer_size, "output write error,should return %d,but returned %d", td.buffer_size, ret_num);

    ret_num = csi_codec_output_write_async(&ch, (void *)0x4, td.buffer_size);
    TEST_CASE_ASSERT(ret_num == 0, "output write error,should return %d,but returned %d", CSI_OK, ret_num);

    ret_sta = csi_codec_output_buffer_reset(&ch);
    TEST_CASE_ASSERT(ret_sta == 0, "output buffer reset error,should return CSI_OK,but returned %d", ret_sta);

    ret_num = csi_codec_output_buffer_avail(&ch);
    TEST_CASE_ASSERT(ret_num == td.buffer_size, "avilable cache size is not equal to the excepted");

    csi_codec_output_close(&ch);
    csi_codec_uninit(&hd);
    free(cache_buffer);

    return 0;
}
