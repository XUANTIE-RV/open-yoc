#include "adc_test.h"
#include <stdlib.h>

void adc_event_cb()
{
    TEST_CASE_TIPS("enter adc cb");
}

int test_adc_interface(char *args)
{
    int32_t ret;
    adc_conf_t adc_config;
    adc_handle_t hd;
    test_adc_args_t td;

    char receive_buffer[32];

//get_capabilities
    adc_capabilities_t cap = drv_adc_get_capabilities(2);

    hd = drv_adc_initialize(td.idx,adc_event_cb);
    TEST_CASE_ASSERT(hd == 0,"hd == NULL act_val = %d",hd);

    hd = drv_adc_initialize(0,NULL);
    TEST_CASE_ASSERT(hd != NULL,"hd != NULL act_val=%d",hd);

    ret = drv_adc_comparator_config(NULL,NULL);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_ADC_BASE | DRV_ERROR_UNSUPPORTED),"ret != (CSI_DRV_ERRNO_ADC_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);

    ret = drv_adc_config(NULL,  &adc_config);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_ADC_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_ADC_BASE | DRV_ERROR_UNSUPPORTED) act_val=%d",ret);


    ret = drv_adc_power_control(NULL,DRV_POWER_FULL);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_ADC_BASE | DRV_ERROR_UNSUPPORTED),"ret != (CSI_DRV_ERRNO_ADC_BASE | DRV_ERROR_UNSUPPORTED) act_val=%d",ret);
    
    ret = drv_adc_power_control(hd,5);
    TEST_CASE_ASSERT(ret != (CSI_DRV_ERRNO_ADC_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_ADC_BASE | DRV_ERROR_UNSUPPORTED) act_val=%d",ret);

    ret = drv_adc_uninitialize(NULL);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_ADC_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_ADC_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);

    ret = drv_adc_start(NULL);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_ADC_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_ADC_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);
    
    ret = drv_adc_start(hd);
    TEST_CASE_ASSERT(ret == 0,"adc failed to start,act_val=%d",ret);

    ret = drv_adc_read(NULL, receive_buffer, 32);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_ADC_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_ADC_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);

    adc_status_t ret_status = drv_adc_get_status(NULL);
    TEST_CASE_ASSERT((ret_status.busy == 0),"ret_status.busy != 0");

    ret = drv_adc_stop(NULL);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_ADC_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_ADC_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);

    ret = drv_adc_stop(hd);
    TEST_CASE_ASSERT(ret == 0,"adc failed to stop,act_val=%d",ret);

    drv_adc_uninitialize(hd);

    return 0;
}













