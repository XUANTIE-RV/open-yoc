/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef _ADC_TEST_H
#define _ADC_TEST_H
#include <stdint.h>
#include <drv/adc.h>
#include <soc.h>
#include <string.h>
#include <stdlib.h>
#include <autotest.h>
#include <test_log.h>
#include <test_config.h>
#include "test_common.h"

#define MAX_CHS 16

typedef struct{
    int32_t idx;         
    uint16_t s_time;     
    bool continue_mode;  
    uint32_t freq_div;   
    uint8_t mode;        
    uint8_t *ch;         
    int num_chs;         
    uint32_t max;        
    uint32_t min;        
    uint16_t *stime_array;
} test_adc_args_t;


extern uint8_t ch_ids[MAX_CHS];
extern uint16_t adc_stime[MAX_CHS];
extern test_adc_args_t test_args;

extern int test_adc_main(char *args);
extern int test_adc_interface(void);
extern int test_adc_status(char *args);
extern int test_adc_async(test_adc_args_t *args,csi_dma_t *dma, csi_dma_ch_t *dma_ch);
extern int test_adc_sync(test_adc_args_t *args);
extern int test_adc_dma(test_adc_args_t *args);
extern int test_adc_stress(void);
extern int adc_pins_write(char *args);
#endif