#ifndef _ADC_TEST_H_
#define _ADC_TEST_H_
#include <stdint.h>
#include <drv/adc.h>
#include <soc.h>
#include <string.h>
#include <stdlib.h>
#include <autotest.h>
#include <test_log.h>
#include <test_config.h>
#include <test_common.h>



typedef struct{
    int32_t idx;         //device id
    bool cmp_mode;      //true to enable the cmp mode, or false to disable
    uint16_t s_time;     //samplling time
    uint32_t freq_div;   //ADC frequence division
//    uint8_t transfer_mode;//0 for polling mode,1 for intrpt mode
    uint8_t mode;        //1: single, 2:continue, 3: scan
    uint8_t *ch;         //channels enabled
    int num_chs;         // num of channels
    uint32_t max;        // expected max value
    uint32_t min;        // expected min value
    uint16_t *stime_array;
    uint16_t count;
} test_adc_args_t;



extern test_adc_args_t test_args;

extern int test_adc_func(char *args);
extern int test_adc_interface(char *args);
extern int test_adc_main(char *args);

#endif