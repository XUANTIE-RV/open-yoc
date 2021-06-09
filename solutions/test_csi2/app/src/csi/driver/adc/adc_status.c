/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include "adc_test.h"


int test_adc_status(char *args)
{
    csi_error_t ret;
    csi_adc_t adc;
    uint32_t freq, freq_read;
    csi_state_t state;
    uint8_t i;
    int32_t data;

    uint8_t ad_idx;
    uint8_t ad_ch;

    uint32_t get_data[3];

    ret = args_parsing(args, get_data, 2);
    TEST_CASE_ASSERT_QUIT(ret == 0, "num_of_parameter error");
    ad_idx = (uint32_t)get_data[0];
    ad_ch = (uint32_t)get_data[1];

    TEST_CASE_TIPS("ADC idx is %d", ad_idx);
    TEST_CASE_TIPS("Channel tested is %d", ad_ch);

    ret = csi_adc_init(&adc, ad_idx);
    TEST_CASE_ASSERT_QUIT(ret == CSI_OK, "ADC init error");

    freq =  csi_adc_freq_div(&adc, 64);
    TEST_CASE_ASSERT(freq > 0, "ADC set freq_div error");
    freq_read =  csi_adc_get_freq(&adc);
    TEST_CASE_ASSERT(freq > 0, "ADC get freq error");
    TEST_CASE_ASSERT(freq == freq_read, "ADC set freq error");
    TEST_CASE_TIPS("Frequence should be %d,but got %d", freq, freq_read);

    freq =  csi_adc_freq_div(&adc, 128);
    TEST_CASE_ASSERT(freq > 0, "ADC set freq error");
    freq_read =  csi_adc_get_freq(&adc);
    TEST_CASE_ASSERT(freq > 0, "ADC get freq error");
    TEST_CASE_ASSERT(freq == freq_read, "ADC set freq err");
    TEST_CASE_TIPS("Frequence should be %d,but got %d", freq, freq_read);

    ret = csi_adc_sampling_time(&adc, 2);
    TEST_CASE_ASSERT(ret == CSI_OK, "ADC set samplling time error");

    ret = csi_adc_continue_mode(&adc, true);
    TEST_CASE_ASSERT(ret == CSI_OK, "ADC set continue_mode error");

    ret = csi_adc_channel_enable(&adc, ad_ch, true);
    TEST_CASE_ASSERT(ret == CSI_OK, "Enable ADC channel error, ch_id: %d", ad_ch);

    ret = csi_adc_get_state(&adc, &state);
    TEST_CASE_ASSERT(ret == CSI_OK, "ADC get state error");
    TEST_CASE_ASSERT(state.writeable == 1, "ADC get state err");
    TEST_CASE_TIPS("adc get state error,the state of adc should be %d", state.writeable);

    ret = csi_adc_start(&adc);
    TEST_CASE_ASSERT(ret == CSI_OK, "ADC start error");

    ret = csi_adc_get_state(&adc, &state);
    TEST_CASE_ASSERT(ret == CSI_OK, "ADC get state error");
    TEST_CASE_ASSERT(state.writeable == 0, "ADC get state error");
    TEST_CASE_TIPS("adc get state error,the state of adc should be %d", state.writeable);

    for (i = 0; i < 5; i++) {
        data = csi_adc_read(&adc);
        TEST_CASE_ASSERT(data >= 0, "ADC read error");
        TEST_CASE_TIPS("the value of adc is %d\n", data);
    }

    ret = csi_adc_channel_enable(&adc, ad_ch, false);
    TEST_CASE_ASSERT(ret == CSI_OK, "Enable ADC channel error, ch_id: %d", ad_ch);

    for (i = 0; i < 5; i++) {
        data = csi_adc_read(&adc);
        TEST_CASE_ASSERT(data < 0, "ADC read error");
        TEST_CASE_TIPS("the value of adc is %d\n", data);
    }

    ret = csi_adc_stop(&adc);
    TEST_CASE_ASSERT(ret == CSI_OK, "ADC stop error");

    ret = csi_adc_get_state(&adc, &state);
    TEST_CASE_ASSERT(ret == CSI_OK, "ADC get state error");
    TEST_CASE_ASSERT(state.writeable == 1, "ADC get state error");
    TEST_CASE_TIPS("adc get state error,the state of adc should be %d", state.writeable);

    csi_adc_uninit(&adc);

    return 0;
}