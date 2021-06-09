/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include "adc_test.h"
volatile uint32_t adc_trigger = 0;
uint8_t ch_ids[MAX_CHS];
uint32_t adc_read_buf[MAX_CHS];
test_adc_args_t test_args;

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

#if defined(ADC_STRESS_STIME)
static int config_s_time(csi_adc_t *adc, uint16_t *stime, uint8_t *ch_ids, int num_chs, uint8_t config_type)
{
    csi_error_t status;
    int i = 0;

    if (config_type == 0) {
        status =  csi_adc_sampling_time(adc, *stime);
        TEST_CASE_ASSERT_QUIT(status == CSI_OK, "ADC set samplling time error");
    } else {
        for (i = 0; i < num_chs; i++) {
            status = csi_adc_channel_sampling_time(adc, *(ch_ids + i), *(stime + i));
            TEST_CASE_ASSERT_QUIT(status == CSI_OK, "ADC set channel samplling time error");
        }
    }

    return 0;
}
#endif


int test_adc_allfunction_sync(test_adc_args_t *args)
{
    uint32_t adc_value;
    int i = 0;
    csi_error_t status;
    csi_adc_t adc_handler;
    status = csi_adc_init(&adc_handler, args->idx);
    TEST_CASE_ASSERT_QUIT(status == CSI_OK, "ADC init error");
    adc_config(&adc_handler, args, NULL, NULL);
    TEST_CASE_READY();

    if ((bool)args->continue_mode == true) {
        status = csi_adc_start(&adc_handler);
        TEST_CASE_ASSERT_QUIT(status == CSI_OK, "ADC start error");
    }

    for (i = 0; i < args->num_chs; i++) {
        if ((bool)args->continue_mode == false) {
            status = csi_adc_start(&adc_handler);
            TEST_CASE_ASSERT_QUIT(status == CSI_OK, "ADC start error");
        }

        adc_value = csi_adc_read(&adc_handler);
        TEST_CASE_ASSERT(adc_value >= test_args.min && adc_value <= test_args.max, "the value of ADC should return between %d and %d, but reutrned %d,and the channel is %d", test_args.min, test_args.max, adc_value, ch_ids[i]);
    }

    status = csi_adc_stop(&adc_handler);
    TEST_CASE_ASSERT(status == CSI_OK, "ADC stop error");
    csi_adc_uninit(&adc_handler);

    return 0;
}

int test_adc_allfunction_async(test_adc_args_t *args, csi_dma_t *dma, csi_dma_ch_t *dma_ch)
{
    int i = 0;
    csi_error_t status;
    csi_adc_t adc_handler;

    status = csi_adc_init(&adc_handler, args->idx);
    TEST_CASE_ASSERT_QUIT(status == CSI_OK, "ADC init error");
    adc_config(&adc_handler, args, dma, dma_ch);

    status = csi_adc_attach_callback(&adc_handler, adc_callback, (void *)&adc_trigger);
    TEST_CASE_ASSERT_QUIT(status == CSI_OK, "ADC attach callback error");

    adc_trigger = 1;
    status = csi_adc_start_async(&adc_handler);
    TEST_CASE_ASSERT_QUIT(status == CSI_OK, "ADC start error");

    while (adc_trigger);

    for (i = 0; i < args->num_chs; i++) {
        TEST_CASE_ASSERT(adc_read_buf[i] >= test_args.min && adc_read_buf[i] <= test_args.max, "the value of ADC should return between %d and %d, but reutrned %d,and the channel is %d", test_args.min, test_args.max, adc_read_buf[i], ch_ids[i]);
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

int test_adc_allfunction_dma(test_adc_args_t *args)
{
    args->mode  = 2;
    csi_dma_t dma;
    csi_dma_ch_t dma_ch;

    test_adc_allfunction_async(args, &dma, &dma_ch);
    return 0;
}


int test_adc_allfunction(char *args)
{
    uint8_t chids[] = {0, 1, 2};
    uint32_t adc_read_buf[MAX_CHS];
    test_args.idx = 0;
    test_args.ch = 0;
    test_args.continue_mode = 0;
    test_args.freq_div = 2;
    test_args.ch = chids;
    test_args.num_chs = sizeof(chids) / sizeof(uint8_t);
    test_args.max = 100;
    test_args.min = 0;


#if defined(ADC_STRESS_STIME)
    uint16_t stime[] = {1, 2, 4};
    test_args.stime_array = stime;
#else
    test_args.s_time = 2;
#endif

    test_args.mode = 0;
    test_adc_allfunction_sync(&test_args);

    test_args.mode = 1;
    test_adc_allfunction_async(&test_args, NULL, NULL);

    test_args.mode = 2;
    test_adc_allfunction_dma(&test_args);

    return 0;

}
