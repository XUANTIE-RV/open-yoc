/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include "adc_test.h"

static void adc_callback(csi_adc_t *adc, csi_adc_event_t event, void *arg)
{
    switch (event) {
        case ADC_EVENT_CONVERT_COMPLETE :
            *((uint32_t *)arg) = 0;
            break;

        case ADC_EVENT_ERROR:
            break;

        default:
            break;
    }
}


int test_adc_dma(char *args)
{
    int i = 0;
    csi_error_t status;
    csi_adc_t adc_handler;
    uint8_t ch_ids[MAX_CHS];
    volatile uint32_t adc_trigger = 0;
    csi_dma_t *dma;
    csi_dma_ch_t *dma_ch;
    test_adc_args_t test_args;
    uint32_t get_data[6];
    uint32_t adc_read_buf[MAX_CHS];

    args_parsing(args, get_data, 6);
    test_args.mode = 1;
    test_args.idx = _dev_idx;
    test_args.s_time = get_data[0];
    test_args.continue_mode = get_data[1];
    test_args.freq_div = get_data[2];
    test_args.max = get_data[3];
    test_args.min = get_data[4];
    ch_ids[0] = get_data[5];
    test_args.num_chs = 1;
    test_args.mode  = 2;
    test_args.ch = ch_ids;

    status = csi_adc_init(&adc_handler, test_args.idx);
    TEST_CASE_ASSERT_QUIT(status == CSI_OK, "ADC init error");
    adc_config(&adc_handler, &test_args, dma, dma_ch);

    status = csi_adc_attach_callback(&adc_handler, adc_callback, (void *)&adc_trigger);
    TEST_CASE_ASSERT_QUIT(status == CSI_OK, "ADC attach callback error");

    adc_trigger = 1;
    status = csi_adc_start_async(&adc_handler);
    TEST_CASE_ASSERT_QUIT(status == CSI_OK, "ADC start error");

    while (adc_trigger);

    for (i = 0; i < test_args.num_chs; i++) {
        TEST_CASE_ASSERT(adc_read_buf[i] >= test_args.min && adc_read_buf[i] <= test_args.max, \
                         "the value of ADC should return between %d and %d, but reutrned %d,and the channel is %d", test_args.min, test_args.max, adc_read_buf[i], ch_ids[i]);
    }

    status = csi_adc_stop_async(&adc_handler);
    TEST_CASE_ASSERT(status == CSI_OK, "ADC async stop error");

    if (dma_ch != NULL) {
        status = csi_adc_link_dma(&adc_handler, NULL);
        TEST_CASE_ASSERT(status == CSI_OK, "unlink DMA error");
    }

    csi_adc_detach_callback(&adc_handler);
    csi_adc_uninit(&adc_handler);
    return 0;
}
