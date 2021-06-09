/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include "adc_test.h"

int test_adc_interface(char *args)
{
    csi_error_t ret;
    csi_adc_t adc;
    int32_t idx = 0;
    uint32_t ret_num = 0;
    csi_state_t state;

    ret = csi_adc_init(NULL, idx);
    TEST_CASE_ASSERT_QUIT(ret == CSI_ERROR, "csi_adc_init test error,should return CSI_ERROR, but returned:%d", ret);

    csi_adc_uninit(NULL);
    ret = csi_adc_set_buffer(NULL, NULL, 0);
    TEST_CASE_ASSERT(ret = CSI_ERROR, "csi_adc_set_buffer test error,should return CSI_ERROR, but returned:%d", ret);

    ret = csi_adc_set_buffer(&adc, NULL, 0);
    TEST_CASE_ASSERT(ret = CSI_ERROR, "csi_adc_set_buffer test error,should return CSI_ERROR, but returned:%d", ret);

    ret = csi_adc_start(NULL);
    TEST_CASE_ASSERT(ret = CSI_ERROR, "csi_adc_start test error,should return CSI_ERROR, but returned:%d", ret);

    ret = csi_adc_start_async(NULL);
    TEST_CASE_ASSERT(ret = CSI_ERROR, "csi_adc_start_async test error,should return CSI_ERROR, but returned:%d", ret);

    ret = csi_adc_stop(NULL);
    TEST_CASE_ASSERT(ret = CSI_ERROR, "csi_adc_stop test error,should return CSI_ERROR, but returned:%d", ret);

    ret = csi_adc_stop_async(NULL);
    TEST_CASE_ASSERT(ret = CSI_ERROR, "csi_adc_stop_async test error,should return CSI_ERROR, but returned:%d", ret);

    ret = csi_adc_channel_enable(NULL, 0, true);
    TEST_CASE_ASSERT(ret == CSI_ERROR, "csi_adc_channel_enable test error,should return CSI_ERROR, but returned:%d", ret);

    ret = csi_adc_channel_sampling_time(NULL, 0, 1);
    TEST_CASE_ASSERT(ret == CSI_ERROR, "csi_adc_channel_sampling_time test error,should return CSI_ERROR, but returned:%d", ret);

    ret = csi_adc_sampling_time(NULL, 1);
    TEST_CASE_ASSERT(ret == CSI_ERROR, "csi_adc_sampling_time test error,should return CSI_ERROR, but returned:%d", ret);

    ret = csi_adc_continue_mode(NULL, true);
    TEST_CASE_ASSERT(ret == CSI_ERROR, "csi_adc_continue_mode test error,should return CSI_ERROR, but returned:%d", ret);

    ret = csi_adc_freq_div(NULL, 0);
    TEST_CASE_ASSERT(ret == CSI_ERROR, "csi_adc_freq_div test error,should return CSI_ERROR, but returned:%d", ret);

    ret_num = csi_adc_read(NULL);
    TEST_CASE_ASSERT(ret_num == CSI_ERROR, "csi_adc_read test error,should return 0, but returned:%d", ret_num);

    ret = csi_adc_get_state(NULL, &state);
    TEST_CASE_ASSERT(ret == CSI_ERROR, "csi_adc_get_state test error,should return CSI_ERROR, but returned:%d", ret);

    ret = csi_adc_attach_callback(NULL, NULL, NULL);
    TEST_CASE_ASSERT(ret == CSI_ERROR, "csi_adc_attach_callback test error,should return CSI_ERROR, but returned:%d", ret);

    csi_adc_detach_callback(NULL);

    ret = csi_adc_link_dma(NULL, NULL);
    TEST_CASE_ASSERT(ret == CSI_ERROR, "csi_adc_link_dma test error,should return CSI_ERROR, but returned:%d", ret);
    return 0;
}