/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <codec_test.h>

volatile static uint8_t reading_lock = 0;

static void input_callback(csi_codec_input_t *input, csi_codec_event_t event, void *arg)
{
    if (event == CODEC_EVENT_PERIOD_READ_COMPLETE) {
        *(uint8_t *)arg += 1;
    }

    if (event == CODEC_EVENT_READ_BUFFER_FULL) {
        reading_lock = 0;
        *(uint8_t *)arg += 1;
    }
}

int test_codec_syncInput(char *args)
{
    csi_error_t ret_sta;
    csi_codec_t hd;
    csi_codec_input_config_t ch_config;
    csi_codec_input_t ch;
    test_codec_args_t td;
    int ret;
    ringbuffer_t input_ring_buffer;

    uint8_t *voice_data = NULL;

    uint32_t get_data[12];

    ret = args_parsing(args, get_data, 12);
    TEST_CASE_ASSERT_QUIT(ret == 0, "num_of_parameter error");

    td.codec_idx = (uint32_t)get_data[0];
    td.ch_idx = (uint32_t)get_data[1];
    td.sample_rate = (uint32_t)get_data[2];
    td.bit_width = (uint32_t)get_data[3];
    td.sound_channel_num = (uint32_t)get_data[4];
    td.data_size = (uint32_t)get_data[5];
    td.buffer_size = (uint32_t)get_data[6];
    td.digital_gain = (uint32_t)get_data[7];
    td.analog_gain = (uint32_t)get_data[8];
    td.mix_gain = (uint32_t)get_data[9];
    td.period = (uint32_t)get_data[10];
    td.mode = (uint32_t)get_data[11];

    TEST_CASE_TIPS("test codec idx is %d", td.codec_idx);
    TEST_CASE_TIPS("test codec channel is %d", td.ch_idx);
    TEST_CASE_TIPS("config sample rate is %d Hz", td.sample_rate);
    TEST_CASE_TIPS("config converter bit width is %d", td.bit_width);
    TEST_CASE_TIPS("config number of soundtrack per channel is %d", td.sound_channel_num);
    TEST_CASE_TIPS("config output data size is %d bytes", td.data_size);
    TEST_CASE_TIPS("config buffer size is %d", td.buffer_size);
    TEST_CASE_TIPS("config digital gain is %d", td.digital_gain);
    TEST_CASE_TIPS("config analog gain is %d", td.analog_gain);
    TEST_CASE_TIPS("config mix gain is %d", td.mix_gain);
    TEST_CASE_TIPS("config priod is %d bytes", td.period);
    TEST_CASE_TIPS("config mode is %d", td.mode);

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
        TEST_CASE_ASSERT_QUIT(1 == 0, "codec %d init error,should return CSI_OK,but retruned %d", td.codec_idx, ret_sta);
    }

    ch.ring_buf = &input_ring_buffer;
    ret_sta = csi_codec_input_open(&hd, &ch, td.ch_idx);

    if (ret_sta != 0) {
        csi_codec_uninit(&hd);
        free(cache_buffer);
        free(voice_data);
        TEST_CASE_ASSERT_QUIT(1 == 0, "input open error,should return CSI_OK,but retruned %d", ret_sta);
    }

    volatile uint8_t irq_num = 1;
    ret_sta = csi_codec_input_attach_callback(&ch, input_callback, (void *)&irq_num);

    if (ret_sta != 0) {
        csi_codec_uninit(&hd);
        free(cache_buffer);
        free(voice_data);
        TEST_CASE_ASSERT_QUIT(1 == 0, "input config error,should return CSI_OK,but retruned %d", ret_sta);
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
        TEST_CASE_ASSERT_QUIT(1 == 0, "input config error,should return CSI_OK,but retruned %d", ret_sta);
    }

    ret_sta = csi_codec_input_digital_gain(&ch, td.digital_gain);

    if ((ret_sta != 0) && (ret_sta != CSI_UNSUPPORTED)) {
        csi_codec_input_close(&ch);
        csi_codec_uninit(&hd);
        free(cache_buffer);
        free(voice_data);
        TEST_CASE_ASSERT_QUIT(1 == 0, "input config digital gain error,should return CSI_OK,but retruned %d", ret_sta);
    }

    ret_sta = csi_codec_input_analog_gain(&ch, td.analog_gain);

    if ((ret_sta != 0) && (ret_sta != CSI_UNSUPPORTED)) {
        csi_codec_input_close(&ch);
        csi_codec_uninit(&hd);
        free(cache_buffer);
        free(voice_data);
        TEST_CASE_ASSERT_QUIT(1 == 0, "input config analog gain error,should return CSI_OK,but retruned %d", ret_sta);
    }

    ret_sta = csi_codec_input_mix_gain(&ch, td.mix_gain);

    if ((ret_sta != 0) && (ret_sta != CSI_UNSUPPORTED)) {
        csi_codec_input_close(&ch);
        csi_codec_uninit(&hd);
        free(cache_buffer);
        free(voice_data);
        TEST_CASE_ASSERT_QUIT(1 == 0, "input config mix gain error,should return CSI_OK,but retruned %d", ret_sta);
    }

    csi_dma_ch_t dma_ch;
    ret_sta = csi_codec_input_link_dma(&ch, &dma_ch);

    if (ret_sta != 0) {
        csi_codec_input_close(&ch);
        csi_codec_uninit(&hd);
        free(cache_buffer);
        free(voice_data);
        TEST_CASE_ASSERT_QUIT(1 == 0, "input link DMA error,should return CSI_OK,but retruned %d", ret_sta);
    }

    ret_sta = csi_codec_input_start(&ch);

    if (ret_sta != 0) {
        csi_codec_input_close(&ch);
        csi_codec_uninit(&hd);
        free(cache_buffer);
        free(voice_data);
        TEST_CASE_ASSERT_QUIT(1 == 0, "input start error,should return CSI_OK,but retruned %d", ret_sta);
    }


    uint32_t ret_num, free_size = 0, use_size = 0;

    reading_lock = 1;
    free_size = td.data_size - use_size;
    ret_num = csi_codec_input_read(&ch, voice_data + use_size, free_size);
    TEST_CASE_ASSERT(ret_num == free_size, "input read error");

    while (reading_lock == 1) {
        tst_mdelay(100);
    }

    csi_codec_input_stop(&ch);

    if ((td.data_size / td.period != 0) && (irq_num == 0)) {
        TEST_CASE_ASSERT(1 == 0, "enter interrupt error");
    }

    ret_sta = csi_codec_input_link_dma(&ch, NULL);
    TEST_CASE_ASSERT(ret_sta == 0, "input unlink dma error");

    csi_codec_input_detach_callback(&ch);

    csi_codec_input_close(&ch);

    csi_codec_uninit(&hd);
    free(cache_buffer);
    free(voice_data);
    return CSI_OK;
}

int test_codec_asyncInput(char *args)
{
    csi_error_t ret_sta;
    csi_codec_t hd;
    csi_codec_input_config_t ch_config;
    csi_codec_input_t ch;
    test_codec_args_t td;
    int ret;
    ringbuffer_t input_ring_buffer;

    uint8_t *voice_data = NULL;

    uint32_t get_data[12];

    ret = args_parsing(args, get_data, 12);
    TEST_CASE_ASSERT_QUIT(ret == 0, "num_of_parameter error");

    td.codec_idx = (uint32_t)get_data[0];
    td.ch_idx = (uint32_t)get_data[1];
    td.sample_rate = (uint32_t)get_data[2];
    td.bit_width = (uint32_t)get_data[3];
    td.sound_channel_num = (uint32_t)get_data[4];
    td.data_size = (uint32_t)get_data[5];
    td.buffer_size = (uint32_t)get_data[6];
    td.digital_gain = (uint32_t)get_data[7];
    td.analog_gain = (uint32_t)get_data[8];
    td.mix_gain = (uint32_t)get_data[9];
    td.period = (uint32_t)get_data[10];
    td.mode = (uint32_t)get_data[11];

    TEST_CASE_TIPS("test codec idx is %d", td.codec_idx);
    TEST_CASE_TIPS("test codec channel is %d", td.ch_idx);
    TEST_CASE_TIPS("config sample rate is %d Hz", td.sample_rate);
    TEST_CASE_TIPS("config converter bit width is %d", td.bit_width);
    TEST_CASE_TIPS("config number of soundtrack per channel is %d", td.sound_channel_num);
    TEST_CASE_TIPS("config output data size is %d bytes", td.data_size);
    TEST_CASE_TIPS("config buffer size is %d", td.buffer_size);
    TEST_CASE_TIPS("config digital gain is %d", td.digital_gain);
    TEST_CASE_TIPS("config analog gain is %d", td.analog_gain);
    TEST_CASE_TIPS("config mix gain is %d", td.mix_gain);
    TEST_CASE_TIPS("config priod is %d bytes", td.period);
    TEST_CASE_TIPS("config mode is %d", td.mode);

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
        TEST_CASE_ASSERT_QUIT(1 == 0, "codec %d init error,should return CSI_OK,but retruned %d", td.codec_idx, ret_sta);
    }

    ch.ring_buf = &input_ring_buffer;
    ret_sta = csi_codec_input_open(&hd, &ch, td.ch_idx);

    if (ret_sta != 0) {
        csi_codec_uninit(&hd);
        free(cache_buffer);
        free(voice_data);
        TEST_CASE_ASSERT_QUIT(1 == 0, "input open error,should return CSI_OK,but retruned %d", ret_sta);
    }

    volatile uint8_t irq_num = 1;
    ret_sta = csi_codec_input_attach_callback(&ch, input_callback, (void *)&irq_num);

    if (ret_sta != 0) {
        csi_codec_uninit(&hd);
        free(cache_buffer);
        free(voice_data);
        TEST_CASE_ASSERT_QUIT(1 == 0, "input config error,should return CSI_OK,but retruned %d", ret_sta);
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
        TEST_CASE_ASSERT_QUIT(1 == 0, "input config error,should return CSI_OK,but retruned %d", ret_sta);
    }

    ret_sta = csi_codec_input_digital_gain(&ch, td.digital_gain);

    if ((ret_sta != 0) && (ret_sta != CSI_UNSUPPORTED)) {
        csi_codec_input_close(&ch);
        csi_codec_uninit(&hd);
        free(cache_buffer);
        free(voice_data);
        TEST_CASE_ASSERT_QUIT(1 == 0, "input config digital gain error,should return CSI_OK,but retruned %d", ret_sta);
    }

    ret_sta = csi_codec_input_analog_gain(&ch, td.analog_gain);

    if ((ret_sta != 0) && (ret_sta != CSI_UNSUPPORTED)) {
        csi_codec_input_close(&ch);
        csi_codec_uninit(&hd);
        free(cache_buffer);
        free(voice_data);
        TEST_CASE_ASSERT_QUIT(1 == 0, "input config analog gain error,should return CSI_OK,but retruned %d", ret_sta);
    }

    ret_sta = csi_codec_input_mix_gain(&ch, td.mix_gain);

    if ((ret_sta != 0) && (ret_sta != CSI_UNSUPPORTED)) {
        csi_codec_input_close(&ch);
        csi_codec_uninit(&hd);
        free(cache_buffer);
        free(voice_data);
        TEST_CASE_ASSERT_QUIT(1 == 0, "input config mix gain error,should return CSI_OK,but retruned %d", ret_sta);
    }

    csi_dma_ch_t dma_ch;
    ret_sta = csi_codec_input_link_dma(&ch, &dma_ch);

    if (ret_sta != 0) {
        csi_codec_input_close(&ch);
        csi_codec_uninit(&hd);
        free(cache_buffer);
        free(voice_data);
        TEST_CASE_ASSERT_QUIT(1 == 0, "input link DMA error,should return CSI_OK,but retruned %d", ret_sta);
    }

    ret_sta = csi_codec_input_start(&ch);

    if (ret_sta != 0) {
        csi_codec_input_close(&ch);
        csi_codec_uninit(&hd);
        free(cache_buffer);
        free(voice_data);
        TEST_CASE_ASSERT_QUIT(1 == 0, "input start error,should return CSI_OK,but retruned %d", ret_sta);
    }

    uint32_t ret_num, free_size = 0, use_size = 0;

    do {
        free_size = td.data_size - use_size;
        reading_lock = 1;
        ret_num = csi_codec_input_read_async(&ch, voice_data + use_size, free_size);
        use_size += ret_num;
    } while (use_size < td.data_size);


    while (reading_lock == 1) {
        tst_mdelay(100);
    }

    csi_codec_input_stop(&ch);

    if ((td.data_size / td.period != 0) && (irq_num == 0)) {
        TEST_CASE_ASSERT(1 == 0, "enter interrrupt error");
    }

    ret_sta = csi_codec_input_link_dma(&ch, NULL);
    TEST_CASE_ASSERT(ret_sta == 0, "input unlink dma error");

    csi_codec_input_detach_callback(&ch);

    csi_codec_input_close(&ch);

    csi_codec_uninit(&hd);
    free(cache_buffer);
    free(voice_data);
    return CSI_OK;
}

int test_codec_inputMute(char *args)
{
    csi_error_t ret_sta;
    csi_codec_t hd;
    csi_codec_input_config_t ch_config;
    csi_codec_input_t ch;
    test_codec_args_t td;
    int ret;
    ringbuffer_t input_ring_buffer;

    uint8_t *voice_data = NULL;

    uint32_t get_data[12];

    ret = args_parsing(args, get_data, 12);
    TEST_CASE_ASSERT_QUIT(ret == 0, "num_of_parameter error");

    td.codec_idx = (uint32_t)get_data[0];
    td.ch_idx = (uint32_t)get_data[1];
    td.sample_rate = (uint32_t)get_data[2];
    td.bit_width = (uint32_t)get_data[3];
    td.sound_channel_num = (uint32_t)get_data[4];
    td.data_size = (uint32_t)get_data[5];
    td.buffer_size = (uint32_t)get_data[6];
    td.digital_gain = (uint32_t)get_data[7];
    td.analog_gain = (uint32_t)get_data[8];
    td.mix_gain = (uint32_t)get_data[9];
    td.period = (uint32_t)get_data[10];
    td.mode = (uint32_t)get_data[11];

    TEST_CASE_TIPS("test codec idx is %d", td.codec_idx);
    TEST_CASE_TIPS("test codec channel is %d", td.ch_idx);
    TEST_CASE_TIPS("config sample rate is %d Hz", td.sample_rate);
    TEST_CASE_TIPS("config converter bit width is %d", td.bit_width);
    TEST_CASE_TIPS("config number of soundtrack per channel is %d", td.sound_channel_num);
    TEST_CASE_TIPS("config output data size is %d bytes", td.data_size);
    TEST_CASE_TIPS("config buffer size is %d", td.buffer_size);
    TEST_CASE_TIPS("config digital gain is %d", td.digital_gain);
    TEST_CASE_TIPS("config analog gain is %d", td.analog_gain);
    TEST_CASE_TIPS("config mix gain is %d", td.mix_gain);
    TEST_CASE_TIPS("config priod is %d bytes", td.period);
    TEST_CASE_TIPS("config mode is %d", td.mode);

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
        TEST_CASE_ASSERT_QUIT(1 == 0, "codec %d init error,should return CSI_OK,but retruned %d", td.codec_idx, ret_sta);
    }

    ch.ring_buf = &input_ring_buffer;
    ret_sta = csi_codec_input_open(&hd, &ch, td.ch_idx);

    if (ret_sta != 0) {
        csi_codec_uninit(&hd);
        free(cache_buffer);
        free(voice_data);
        TEST_CASE_ASSERT_QUIT(1 == 0, "input open error,should return CSI_OK,but retruned %d", ret_sta);
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
        TEST_CASE_ASSERT_QUIT(1 == 0, "input config error,should return CSI_OK,but retruned %d", ret_sta);
    }

    ret_sta = csi_codec_input_digital_gain(&ch, td.digital_gain);

    if ((ret_sta != 0) && (ret_sta != CSI_UNSUPPORTED)) {
        csi_codec_input_close(&ch);
        csi_codec_uninit(&hd);
        free(cache_buffer);
        free(voice_data);
        TEST_CASE_ASSERT_QUIT(1 == 0, "input config digital gain error,should return CSI_OK,but retruned %d", ret_sta);
    }

    ret_sta = csi_codec_input_analog_gain(&ch, td.analog_gain);

    if ((ret_sta != 0) && (ret_sta != CSI_UNSUPPORTED)) {
        csi_codec_input_close(&ch);
        csi_codec_uninit(&hd);
        free(cache_buffer);
        free(voice_data);
        TEST_CASE_ASSERT_QUIT(1 == 0, "input config analog gain error,should return CSI_OK,but retruned %d", ret_sta);
    }

    ret_sta = csi_codec_input_mix_gain(&ch, td.mix_gain);

    if ((ret_sta != 0) && (ret_sta != CSI_UNSUPPORTED)) {
        csi_codec_input_close(&ch);
        csi_codec_uninit(&hd);
        free(cache_buffer);
        free(voice_data);
        TEST_CASE_ASSERT_QUIT(1 == 0, "input config mix gain error,should return CSI_OK,but retruned %d", ret_sta);
    }

    csi_dma_ch_t dma_ch;

    ret_sta = csi_codec_input_link_dma(&ch, &dma_ch);

    if (ret_sta != 0) {
        csi_codec_input_close(&ch);
        csi_codec_uninit(&hd);
        free(cache_buffer);
        free(voice_data);
        TEST_CASE_ASSERT_QUIT(1 == 0, "input link DMA error,should return CSI_OK,but retruned %d", ret_sta);
    }

    ret_sta = csi_codec_input_start(&ch);

    if (ret_sta != 0) {
        csi_codec_input_close(&ch);
        csi_codec_uninit(&hd);
        free(cache_buffer);
        free(voice_data);
        TEST_CASE_ASSERT_QUIT(1 == 0, "input start error,should return CSI_OK,but retruned %d", ret_sta);
    }

    tst_mdelay(100);

    ret_sta = csi_codec_input_mute(&ch, true);
    TEST_CASE_ASSERT_QUIT(ret_sta == 0, "input mute error,should return CSI_OK,but retruned %d", ret_sta);

    uint32_t ret_num, free_size = 0, use_size = 0;

    do {
        free_size = td.data_size - use_size;
        ret_num = csi_codec_input_read_async(&ch, voice_data + use_size, free_size);
        use_size += ret_num;
    } while (use_size < td.data_size);

    csi_codec_input_stop(&ch);

    ret_sta = csi_codec_input_link_dma(&ch, NULL);
    TEST_CASE_ASSERT(ret_sta == 0, "input unlink dma error");

    csi_codec_input_detach_callback(&ch);

    csi_codec_input_close(&ch);

    csi_codec_uninit(&hd);
    free(cache_buffer);
    free(voice_data);

    return CSI_OK;
}
