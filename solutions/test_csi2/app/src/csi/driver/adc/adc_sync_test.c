/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include "adc_test.h"


int test_adc_sync(char *args)
{
    uint32_t adc_value;
    int i = 0;
    csi_error_t status;
    csi_adc_t adc_handler;
    test_adc_args_t test_args;
    uint8_t ch_ids[MAX_CHS];
    uint32_t get_data[6];

    args_parsing(args, get_data, 6);
    test_args.mode = 0;
    test_args.idx = _dev_idx;
    test_args.s_time = get_data[0];
    test_args.continue_mode = get_data[1];
    test_args.freq_div = get_data[2];
    test_args.max = get_data[3];
    test_args.min = get_data[4];
    ch_ids[0] = get_data[5];
    test_args.num_chs = 1;
    test_args.ch = ch_ids;

    status = csi_adc_init(&adc_handler, test_args.idx);
    TEST_CASE_ASSERT_QUIT(status == CSI_OK, "ADC init error");
    adc_config(&adc_handler, &test_args, NULL, NULL);

    TEST_CASE_READY();

    if ((bool)test_args.continue_mode == true) {
        status = csi_adc_start(&adc_handler);
        TEST_CASE_ASSERT_QUIT(status == CSI_OK, "ADC start error");
    }

    for (i = 0; i < test_args.num_chs; i++) {
        if ((bool)test_args.continue_mode == false) {
            status = csi_adc_start(&adc_handler);
            TEST_CASE_ASSERT_QUIT(status == CSI_OK, "ADC start error");
        }

        adc_value = csi_adc_read(&adc_handler);
        TEST_CASE_ASSERT(adc_value >= test_args.min && adc_value <= test_args.max, "the value of ADC should return between %d and %d, but reutrned:%d,and the channel is %d", test_args.min, test_args.max, adc_value, ch_ids[i]);
    }

    status = csi_adc_stop(&adc_handler);
    TEST_CASE_ASSERT(status == CSI_OK, "ADC stop error");
    csi_adc_uninit(&adc_handler);

    return 0;
}