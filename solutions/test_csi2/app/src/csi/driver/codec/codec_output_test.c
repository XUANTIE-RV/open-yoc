/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <codec_test.h>

volatile static uint8_t writeing_lock = 0;

static void output_callback(csi_codec_output_t *output, csi_codec_event_t event, void *arg)
{
    if (event == CODEC_EVENT_PERIOD_WRITE_COMPLETE) {
        *(uint8_t *)arg += 1;
    }

    if (event == CODEC_EVENT_WRITE_BUFFER_EMPTY) {
        writeing_lock = 0;
        *(uint8_t *)arg += 1;
    }
}

int test_codec_syncOutput(char *args)
{
    csi_error_t ret_sta;
    csi_codec_t hd;
    csi_codec_output_config_t ch_config;
    csi_codec_output_t ch;
    test_codec_args_t td;
    int ret;
    ringbuffer_t output_ring_buffer;

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

    volatile uint8_t irq_num = 0;
    ret_sta = csi_codec_output_attach_callback(&ch, output_callback, (void *)&irq_num);

    if (ret_sta != 0) {
        csi_codec_uninit(&hd);
        free(cache_buffer);
        TEST_CASE_ASSERT_QUIT(1 == 0, "output config error,should return CSI_OK,but returned %d", ret_sta);
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

    uint32_t ret_num, free_size = 0, use_size = 0;

    writeing_lock = 1;
    free_size = td.data_size - use_size;
    ret_num = csi_codec_output_write(&ch, (const void *)0x4, free_size);
    TEST_CASE_ASSERT(ret_num == free_size, "output write error");

    while (writeing_lock == 1) {
        tst_mdelay(100);
    }

    if ((td.data_size / td.period != 0) && (irq_num == 0)) {
        TEST_CASE_ASSERT(1 == 0, "enter interrupt error");
    }

    csi_codec_output_stop(&ch);

    ret_sta = csi_codec_output_link_dma(&ch, NULL);
    TEST_CASE_ASSERT(ret_sta == 0, "output unlink dma error");

    csi_codec_output_detach_callback(&ch);

    csi_codec_output_close(&ch);

    csi_codec_uninit(&hd);
    free(cache_buffer);

    return 0;
}

int test_codec_outputMute(char *args)
{
    csi_error_t ret_sta;
    csi_codec_t hd;
    csi_codec_output_config_t ch_config;
    csi_codec_output_t ch;
    test_codec_args_t td;
    int ret;
    ringbuffer_t output_ring_buffer;

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
        TEST_CASE_ASSERT_QUIT(1 == 0, "output link DMA error");
    }

    ret_sta = csi_codec_output_start(&ch);

    if (ret_sta != 0) {
        csi_codec_output_close(&ch);
        csi_codec_uninit(&hd);
        free(cache_buffer);
        TEST_CASE_ASSERT_QUIT(1 == 0, "output start error,should return CSI_OK,but returned %d", ret_sta);
    }

    tst_mdelay(100);

    ret_sta = csi_codec_output_mute(&ch, true);
    TEST_CASE_ASSERT(ret_sta == 0, "output mute error,should return CSI_OK,but returned %d", ret_sta);

    uint32_t ret_num, free_size = 0, use_size = 0;

    do {
        free_size = td.data_size - use_size;
        ret_num = csi_codec_output_write_async(&ch, (const void *)0x4, free_size);
        use_size += ret_num;
    } while (use_size < td.data_size);

    csi_codec_output_stop(&ch);

    ret_sta = csi_codec_output_link_dma(&ch, NULL);
    TEST_CASE_ASSERT(ret_sta == 0, "output unlink dma error");

    csi_codec_output_detach_callback(&ch);

    csi_codec_output_close(&ch);

    csi_codec_uninit(&hd);
    free(cache_buffer);

    return 0;
}

int test_codec_asyncOutput(char *args)
{
    csi_error_t ret_sta;
    csi_codec_t hd;
    csi_codec_output_config_t ch_config;
    csi_codec_output_t ch;
    test_codec_args_t td;
    int ret;
    ringbuffer_t output_ring_buffer;

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

    volatile uint8_t irq_num = 0;
    ret_sta = csi_codec_output_attach_callback(&ch, output_callback, (void *)&irq_num);

    if (ret_sta != 0) {
        csi_codec_uninit(&hd);
        free(cache_buffer);
        TEST_CASE_ASSERT_QUIT(1 == 0, "output config error,should return CSI_OK,but returned %d", ret_sta);
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
        TEST_CASE_ASSERT_QUIT(1 == 0, "output link DMA error");
    }

    ret_sta = csi_codec_output_start(&ch);

    if (ret_sta != 0) {
        csi_codec_output_close(&ch);
        csi_codec_uninit(&hd);
        free(cache_buffer);
        TEST_CASE_ASSERT_QUIT(1 == 0, "output start error,should return CSI_OK,but returned %d", ret_sta);
    }

    uint32_t ret_num, free_size = 0, use_size = 0;

    do {
        free_size = td.data_size - use_size;
        writeing_lock = 1;
        ret_num = csi_codec_output_write_async(&ch, (const void *)0x4, free_size);
        use_size += ret_num;
    } while (use_size < td.data_size);

    while (writeing_lock == 1) {
        tst_mdelay(100);
    }

    if ((td.data_size / td.period != 0) && (irq_num == 0)) {
        TEST_CASE_ASSERT(1 == 0, "enter interrupt error");
    }

    csi_codec_output_stop(&ch);

    ret_sta = csi_codec_output_link_dma(&ch, NULL);
    TEST_CASE_ASSERT(ret_sta == 0, "output unlink dma error");

    csi_codec_output_detach_callback(&ch);

    csi_codec_output_close(&ch);

    csi_codec_uninit(&hd);
    free(cache_buffer);

    return 0;
}
