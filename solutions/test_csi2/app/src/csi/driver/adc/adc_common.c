/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include "adc_test.h"

int adc_config(csi_adc_t *adc_handler, test_adc_args_t *args, csi_dma_t *dma, csi_dma_ch_t *dma_ch)
{
    int i;
    csi_error_t status;

    status =  csi_adc_continue_mode(adc_handler, (bool)args->continue_mode);
    TEST_CASE_ASSERT_QUIT(status == CSI_OK, "ADC set continue_mode error");

    uint32_t freq;
    freq =  csi_adc_freq_div(adc_handler, args->freq_div);
    TEST_CASE_ASSERT_QUIT(freq > 0, "ADC set freq error");

#if defined(ADC_STRESS_STIME)
    int ret = 0;
    ret = config_s_time(adc_handler, args->stime_array, args->ch, args->num_chs, 1);
    TEST_CASE_ASSERT_QUIT(ret == 0, "config samplling time error.");
#else
    status =  csi_adc_sampling_time(adc_handler, args->s_time);
    TEST_CASE_ASSERT_QUIT(status == CSI_OK, "ADC set samplling time error");
#endif

    if (args->mode == 1 || args->mode == 2) {
        status = csi_adc_set_buffer(adc_handler, adc_read_buf, args->num_chs);
        TEST_CASE_ASSERT_QUIT(status == CSI_OK, "ADC set buffer error");
    }

    if (args->mode == 2) {
        status = csi_adc_link_dma(adc_handler, dma_ch);
        TEST_CASE_ASSERT_QUIT(status == CSI_OK, "ADC link DMA error");

        status = csi_adc_set_buffer(adc_handler, adc_read_buf, args->num_chs);
        TEST_CASE_ASSERT_QUIT(status == CSI_OK, "ADC set buffer error");
    }

    for (i = 0; i < args->num_chs; i++) {
        status = csi_adc_channel_enable(adc_handler, *(args->ch + i), true);
        TEST_CASE_ASSERT_QUIT(status == CSI_OK, "enable ADC channel error, ch_id: %d", *(args->ch + i));
    }

    return 0;
}