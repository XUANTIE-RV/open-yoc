#include <gpio_test.h>

void gpio_event_cb()
{
	TEST_CASE_TIPS("enter gpio cb");
}


int test_gpio_interface(char *args)
{
	int ret;
	test_gpio_args_t td;
	gpio_pin_handle_t hd;


//	hd = csi_gpio_pin_initialize(0,gpio_event_cb);
//    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_GPIO_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_GPIO_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);

	hd = csi_gpio_pin_initialize(4,gpio_event_cb);
	TEST_CASE_ASSERT(hd != NULL,"hd == NULL act_val=%d",hd);

//	ret = drv_adc_power_control(NULL,DRV_POWER_FULL);
//    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_GPIO_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_GPIO_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);
    
    ret = csi_gpio_power_control(hd,5);
    TEST_CASE_ASSERT(ret != (CSI_DRV_ERRNO_GPIO_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_GPIO_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);

	ret = csi_gpio_pin_uninitialize(NULL);
	TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_GPIO_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_GPIO_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);

	ret = csi_gpio_pin_config_direction(NULL,GPIO_DIRECTION_INPUT );
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_GPIO_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_GPIO_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);

	ret = csi_gpio_pin_config_mode(NULL,GPIO_MODE_PULLNONE);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_GPIO_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_GPIO_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);

	ret = csi_gpio_pin_write(NULL,1);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_GPIO_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_GPIO_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);

	ret = csi_gpio_pin_read(NULL,1);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_GPIO_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_GPIO_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);

	csi_gpio_pin_uninitialize(hd);

	return 0;
}

#if 0
#include <gpio_test.h>

void gpio_event_cb()
{
	TEST_CASE_TIPS("enter gpio cb");
}

int test_gpio_interface(char *args)
{
	int ret;
	test_gpio_args_t td;
	gpio_pin_handle_t hd;

	hd = csi_gpio_pin_initialize(0,gpio_event_cb);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_GPIO_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_GPIO_BASE | DRV_ERROR_UNSUPPORTED) act_val=%d",ret);

	hd = csi_gpio_pin_initialize(td.pin,gpio_event_cb);

	ret = drv_adc_power_control(NULL,DRV_POWER_FULL);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_GPIO_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_GPIO_BASE | DRV_ERROR_UNSUPPORTED) act_val=%d",ret);
    
    ret = drv_adc_power_control(hd,5);
    TEST_CASE_ASSERT(ret != (CSI_DRV_ERRNO_GPIO_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_GPIO_BASE | DRV_ERROR_UNSUPPORTED) act_val=%d",ret);

	ret = csi_gpio_pin_uninitialize(NULL);
	TEST_CASE_ASSERT(ret != (CSI_DRV_ERRNO_GPIO_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_GPIO_BASE | DRV_ERROR_UNSUPPORTED) act_val=%d",ret);

	ret = csi_gpio_pin_config_direction(NULL,td.dir);
    TEST_CASE_ASSERT(ret != (CSI_DRV_ERRNO_GPIO_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_GPIO_BASE | DRV_ERROR_UNSUPPORTED) act_val=%d",ret);

	ret = csi_gpio_pin_config_mode(NULL,td.gpio_mode);
    TEST_CASE_ASSERT(ret != (CSI_DRV_ERRNO_GPIO_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_GPIO_BASE | DRV_ERROR_UNSUPPORTED) act_val=%d",ret);

	ret = csi_gpio_pin_write(NULL,td.pin_value);
    TEST_CASE_ASSERT(ret != (CSI_DRV_ERRNO_GPIO_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_GPIO_BASE | DRV_ERROR_UNSUPPORTED) act_val=%d",ret);

	ret = csi_gpio_pin_read(NULL,td.pin_value);
    TEST_CASE_ASSERT(ret != (CSI_DRV_ERRNO_GPIO_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_GPIO_BASE | DRV_ERROR_UNSUPPORTED) act_val=%d",ret);

	drv_gpio_pin_uninitialize(hd);

	return 0;
}

#endif

