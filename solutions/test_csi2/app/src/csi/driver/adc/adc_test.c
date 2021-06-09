/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <adc_test.h>

test_func_map_t adc_test_funcs_map[] = {
    {"ADC_INTERFACE", test_adc_interface},
    {"ADC_STATUS", test_adc_status},
    {"ADC_SYNC", test_adc_sync},
    {"ADC_ASYNC", test_adc_async},
    {"ADC_DMA", test_adc_dma},
    {"ADC_MULTIFUNC", test_adc_allfunction},
};



int test_adc_main(char *args)
{
    uint8_t i;

    for (i = 0; i < sizeof(adc_test_funcs_map) / sizeof(test_func_map_t); i++) {
        if (!strcmp((void *)_mc_name, adc_test_funcs_map[i].test_func_name)) {
            (*(adc_test_funcs_map[i].test_func_p))(args);
            return 0;
        }
    }

    TEST_CASE_TIPS("ADC module don't support this command.");
    return -1;
}

