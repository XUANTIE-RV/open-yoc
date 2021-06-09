/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <codec_test.h>

int test_codec_outputChannelState(char *args)
{
    csi_error_t ret_sta;
    csi_codec_t hd;
    csi_codec_output_config_t ch_config;
    csi_codec_output_t ch;
    test_codec_args_t td;
    int ret;
    ringbuffer_t output_ring_buffer;

    csi_state_t state;

    uint32_t get_data[2];

    ret = args_parsing(args, get_data, 2);
    TEST_CASE_ASSERT_QUIT(ret == 0, "num_of_parameter error");

    td.codec_idx = (uint32_t)get_data[0];
    td.ch_idx = (uint32_t)get_data[1];
    td.sample_rate = 48000;
    td.bit_width = 16;
    td.sound_channel_num = 1;
    td.data_size = 2048;
    td.buffer_size = 1024;
    td.digital_gain = 0;
    td.analog_gain = 46;
    td.mix_gain = 0;
    td.period = 512;
    td.mode = 1;


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
        TEST_CASE_ASSERT_QUIT(1 == 0, "output %d open error,should return CSI_OK,but returned %d", td.ch_idx, ret_sta);
    }


    ret_sta = csi_codec_output_get_state(&ch, &state);
    TEST_CASE_ASSERT(ret_sta == 0, "output get state error,should return CSI_OK,but returned %d", ret_sta);

    if ((state.readable != 0) || (state.writeable != 0)) {
        TEST_CASE_ASSERT(1 == 0, "output get wrong state");
    }

    ch_config.sample_rate = td.sample_rate;
    ch_config.bit_width = td.bit_width;
    ch_config.buffer = cache_buffer;
    ch_config.buffer_size = td.buffer_size;
    ch_config.sound_channel_num = td.sound_channel_num;
    ch_config.period = td.period;
    ch_config.mode = td.mode;

    ret_sta = csi_codec_output_config(&ch, &ch_config);

    if (ret_sta != 0) {
        csi_codec_output_close(&ch);
        csi_codec_uninit(&hd);
        free(cache_buffer);
        TEST_CASE_ASSERT_QUIT(1 == 0, "output config error,should return CSI_OK,but returned %d", ret_sta);
    }

    ret_sta = csi_codec_output_digital_gain(&ch, td.digital_gain);

    if ((ret_sta != 0) && (ret_sta != CSI_UNSUPPORTED)) {
        csi_codec_output_close(&ch);
        csi_codec_uninit(&hd);
        free(cache_buffer);
        TEST_CASE_ASSERT_QUIT(1 == 0, "output config digital gain error,should return CSI_OK,but returned %d", ret_sta);
    }

    ret_sta = csi_codec_output_analog_gain(&ch, td.analog_gain);

    if ((ret_sta != 0) && (ret_sta != CSI_UNSUPPORTED)) {
        csi_codec_output_close(&ch);
        csi_codec_uninit(&hd);
        free(cache_buffer);
        TEST_CASE_ASSERT_QUIT(1 == 0, "output config analog gain error,should return CSI_OK,but returned %d", ret_sta);
    }

    ret_sta = csi_codec_output_mix_gain(&ch, td.mix_gain);

    if ((ret_sta != 0) && (ret_sta != CSI_UNSUPPORTED)) {
        csi_codec_output_close(&ch);
        csi_codec_uninit(&hd);
        free(cache_buffer);
        TEST_CASE_ASSERT_QUIT(1 == 0, "output config mix gain error,should return CSI_OK,but returned %d", ret_sta);
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

    ret_num = csi_codec_output_write(&ch, (const void *)0x4, td.data_size);
    TEST_CASE_ASSERT(ret_num == td.data_size, "output write error");

    ret_sta = csi_codec_output_get_state(&ch, &state);
    TEST_CASE_ASSERT(ret_sta == 0, "output get state error,should return CSI_OK,but returned %d", ret_sta);

    if ((state.readable != 0) || (state.writeable == 0)) {
        TEST_CASE_ASSERT(1 == 0, "output get wrong state");
    }

    csi_codec_output_stop(&ch);

    ret_sta = csi_codec_output_get_state(&ch, &state);
    TEST_CASE_ASSERT(ret_sta == 0, "output get state error,should return CSI_OK,but returned %d", ret_sta);

    if ((state.readable != 0) || (state.writeable != 0)) {
        TEST_CASE_ASSERT(1 == 0, "output get wrong state");
    }

    ret_sta = csi_codec_output_link_dma(&ch, NULL);
    TEST_CASE_ASSERT(ret_sta == 0, "output unlink dma error");


    csi_codec_output_close(&ch);

    ret_sta = csi_codec_output_get_state(&ch, &state);
    TEST_CASE_ASSERT(ret_sta == 0, "output get state error,should return CSI_OK,but returned %d", ret_sta);

    if ((state.readable != 0) || (state.writeable != 0)) {
        TEST_CASE_ASSERT(1 == 0, "output get wrong state");
    }

    ret_sta = csi_codec_output_link_dma(&ch, NULL);
    TEST_CASE_ASSERT(ret_sta == CSI_ERROR, "output unlink dma error");

    csi_codec_uninit(&hd);
    free(cache_buffer);

    return CSI_OK;
}



int test_codec_inputChannelState(char *args)
{
    csi_error_t ret_sta;
    csi_codec_t hd;
    csi_codec_input_config_t ch_config;
    csi_codec_input_t ch;
    test_codec_args_t td;
    int ret;
    ringbuffer_t input_ring_buffer;
    csi_state_t state;

    uint8_t *voice_data = NULL;

    uint32_t get_data[2];

    ret = args_parsing(args, get_data, 2);
    TEST_CASE_ASSERT_QUIT(ret == 0, "num_of_parameter error");

    td.codec_idx = (uint32_t)get_data[0];
    td.ch_idx = (uint32_t)get_data[1];
    td.sample_rate = 48000;
    td.bit_width = 16;
    td.sound_channel_num = 1;
    td.data_size = 1024;
    td.buffer_size = 512;
    td.digital_gain = 0;
    td.analog_gain = 46;
    td.mix_gain = 0;
    td.period = 256;
    td.mode = 1;

    TEST_CASE_TIPS("test codec idx is %d", td.codec_idx);
    TEST_CASE_TIPS("test codec channel is %d", td.ch_idx);


    if (voice_data == NULL) {
        voice_data = malloc(td.data_size);

        if (voice_data == NULL) {
            TEST_CASE_WARN_QUIT("malloc error");
        }
    }

    uint8_t *cache_buffer = NULL;
    cache_buffer = malloc(td.buffer_size);

    if (cache_buffer == NULL) {
        free(voice_data);
        TEST_CASE_WARN_QUIT("malloc error");
    }

    ret_sta = csi_codec_init(&hd, td.codec_idx);

    if (ret_sta != 0) {
        free(cache_buffer);
        free(voice_data);
        TEST_CASE_ASSERT_QUIT(1 == 0, "codec %d init error,should return CSI_OK,but returned %d", td.codec_idx, ret_sta);
    }

    ch.ring_buf = &input_ring_buffer;
    ret_sta = csi_codec_input_open(&hd, &ch, td.ch_idx);

    if (ret_sta != 0) {
        csi_codec_uninit(&hd);
        free(cache_buffer);
        free(voice_data);
        TEST_CASE_ASSERT_QUIT(1 == 0, "input open error,should return CSI_OK,but returned %d", ret_sta);
    }

    ret_sta = csi_codec_input_get_state(&ch, &state);
    TEST_CASE_ASSERT(ret_sta == 0, "input get state error,should return CSI_OK,but returned %d", ret_sta);

    if ((state.readable != 0) || (state.writeable != 0)) {
        TEST_CASE_ASSERT(1 == 0, "input get wrong state");
    }

    ch_config.sample_rate = td.sample_rate;
    ch_config.bit_width = td.bit_width;
    ch_config.buffer = cache_buffer;
    ch_config.buffer_size = td.buffer_size;
    ch_config.sound_channel_num = td.sound_channel_num;
    ch_config.period = td.period;
    ch_config.mode = td.mode;

    ret_sta = csi_codec_input_config(&ch, &ch_config);

    if (ret_sta != 0) {
        csi_codec_input_close(&ch);
        csi_codec_uninit(&hd);
        free(cache_buffer);
        free(voice_data);
        TEST_CASE_ASSERT_QUIT(1 == 0, "input config error,should return CSI_OK,but returned %d", ret_sta);
    }

    ret_sta = csi_codec_input_digital_gain(&ch, td.digital_gain);

    if ((ret_sta != 0) && (ret_sta != CSI_UNSUPPORTED)) {
        csi_codec_input_close(&ch);
        csi_codec_uninit(&hd);
        free(cache_buffer);
        free(voice_data);
        TEST_CASE_ASSERT_QUIT(1 == 0, "input config digital gain error,should return CSI_OK,but returned %d", ret_sta);
    }

    ret_sta = csi_codec_input_analog_gain(&ch, td.analog_gain);

    if ((ret_sta != 0) && (ret_sta != CSI_UNSUPPORTED)) {
        csi_codec_input_close(&ch);
        csi_codec_uninit(&hd);
        free(cache_buffer);
        free(voice_data);
        TEST_CASE_ASSERT_QUIT(1 == 0, "input config analog gain error,should return CSI_OK,but returned %d", ret_sta);
    }

    ret_sta = csi_codec_input_mix_gain(&ch, td.mix_gain);

    if ((ret_sta != 0) && (ret_sta != CSI_UNSUPPORTED)) {
        csi_codec_input_close(&ch);
        csi_codec_uninit(&hd);
        free(cache_buffer);
        free(voice_data);
        TEST_CASE_ASSERT_QUIT(1 == 0, "input config mix gain error,should return CSI_OK,but returned %d", ret_sta);
    }


    csi_dma_ch_t dma_ch;

    ret_sta = csi_codec_input_link_dma(&ch, &dma_ch);

    if (ret_sta != 0) {
        csi_codec_input_close(&ch);
        csi_codec_uninit(&hd);
        free(cache_buffer);
        free(voice_data);
        TEST_CASE_ASSERT_QUIT(1 == 0, "input link DMA error,should return CSI_OK,but returned %d", ret_sta);
    }

    ret_sta = csi_codec_input_start(&ch);

    if (ret_sta != 0) {
        csi_codec_input_close(&ch);
        csi_codec_uninit(&hd);
        free(cache_buffer);
        free(voice_data);
        TEST_CASE_ASSERT_QUIT(1 == 0, "input start error,should return CSI_OK,but returned %d", ret_sta);
    }

    uint32_t ret_num, free_size = 0, use_size = 0;

    free_size = td.data_size - use_size;
    ret_num = csi_codec_input_read(&ch, voice_data + use_size, free_size);
    TEST_CASE_ASSERT(ret_num == free_size, "input read error");

    ret_sta = csi_codec_input_get_state(&ch, &state);
    TEST_CASE_ASSERT(ret_sta == 0, "input get state error,should return CSI_OK,but returned %d", ret_sta);

    if ((state.readable == 0) || (state.writeable != 0)) {
        TEST_CASE_ASSERT(1 == 0, "input get wrong state");
    }


    csi_codec_input_stop(&ch);

    ret_sta = csi_codec_input_get_state(&ch, &state);
    TEST_CASE_ASSERT(ret_sta == 0, "input get state error,should return CSI_OK,but returned %d", ret_sta);

    if ((state.readable != 0) || (state.writeable != 0)) {
        TEST_CASE_ASSERT(1 == 0, "input get wrong state");
    }


    ret_sta = csi_codec_input_link_dma(&ch, NULL);
    TEST_CASE_ASSERT(ret_sta == 0, "input unlink dma error");


    csi_codec_input_close(&ch);

    if ((state.readable != 0) || (state.writeable != 0)) {
        TEST_CASE_ASSERT(1 == 0, "input get wrong state");
    }


    ret_sta = csi_codec_input_link_dma(&ch, NULL);
    TEST_CASE_ASSERT(ret_sta == CSI_ERROR, "input unlink dma error");

    csi_codec_uninit(&hd);
    free(cache_buffer);
    free(voice_data);
    return CSI_OK;
}







