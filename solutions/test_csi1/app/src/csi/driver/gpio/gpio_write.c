#include <gpio_test.h>

int test_gpio_pinWrite(char *args)
{

    test_gpio_args_t td;
    gpio_pin_handle_t hd;

    int ret;


//赋值
    td.dir = GPIO_DIRECTION_OUTPUT;
	td.pin = (uint8_t)*((uint32_t *)args);
	td.gpio_mode =*((uint32_t *)args+1);
	td.pin_value  = (uint8_t)*((uint32_t *)args+2);
	td.delay_ms = (uint8_t)*((uint32_t *)args+3);

//打印一下
//    TEST_CASE_TIPS("test GPIO idx is %d", td.gpio_idx);
/*
    TEST_CASE_TIPS("config pins is %x", td.pin);
    TEST_CASE_TIPS("config direction is %d", td.dir); 
    TEST_CASE_TIPS("config mode is %d", td.gpio_mode);
    TEST_CASE_TIPS("expected write value is %d", td.pin_value);
	TEST_CASE_TIPS("output delay %d ms", td.delay_ms);
*/
    hd = csi_gpio_pin_initialize(td.pin,NULL);
    TEST_CASE_ASSERT_QUIT(hd != NULL, "gpio init fail.\n");
    
    ret = csi_gpio_pin_config_direction(hd,td.dir);
    TEST_CASE_ASSERT_QUIT(ret == 0, "gpio direction config error: expected return value is 0,actual return value is %d.", ret);
    
    ret = csi_gpio_pin_config_mode(hd,td.gpio_mode);
    TEST_CASE_ASSERT_QUIT(ret == 0, "gpio mode config error: expected return value is 0,actual return value is %d.", ret);
    
	ret = csi_gpio_pin_write(hd,td.pin_value);
    TEST_CASE_ASSERT_QUIT(ret == 0, "gpio write error: expected return value is 0,actual return value is %d.", ret);
	

	tst_mdelay(50);

	TEST_CASE_READY();

	tst_mdelay(td.delay_ms);
    ret = csi_gpio_pin_uninitialize(hd);
    TEST_CASE_ASSERT_QUIT(ret == 0, "gpio uninit fail.\n");
	return 0;
}

/*
int test_gpio_pinsWrite(char *args)
{
    test_gpio_args_t td;
    gpio_pin_handle_t hd;

    int ret;

//赋值
    td.dir = GPIO_DIRECTION_OUTPUT;
	td.pin_mask = (uint8_t)*((uint32_t *)args);
	td.gpio_mode =*((uint32_t *)args+1);
	td.pin_value  = (uint8_t)*((uint32_t *)args+2);
	td.delay_ms = (uint8_t)*((uint32_t *)args+3);

//打印一下
    TEST_CASE_TIPS("config pins is 0x%x", td.pin_mask);
    TEST_CASE_TIPS("config direction is %d", td.dir); 
    TEST_CASE_TIPS("config mode is %d", td.gpio_mode);
    TEST_CASE_TIPS("expected write value is %d", td.pin_value);
	TEST_CASE_TIPS("output delay %d ms", td.delay_ms);

    hd = csi_gpio_pin_initialize(td.pin_mask,NULL);
    TEST_CASE_ASSERT_QUIT(hd != NULL, "gpio init fail.\n");
    
    ret = csi_gpio_pin_config_direction(hd,td.dir);
    TEST_CASE_ASSERT_QUIT(ret == 0, "gpio direction config error: expected return value is 0,actual return value is %d.", ret);
    
    ret = csi_gpio_pin_config_mode(hd,td.gpio_mode);
    TEST_CASE_ASSERT_QUIT(ret == 0, "gpio mode config error: expected return value is 0,actual return value is %d.", ret);
    
	ret = csi_gpio_pin_write(hd,td.pin_value);
    TEST_CASE_ASSERT_QUIT(ret == 0, "gpio write error: expected return value is 0,actual return value is %d.", ret);
	
	tst_mdelay(50);

	TEST_CASE_READY();

	tst_mdelay(td.delay_ms);
    csi_gpio_pin_uninitialize(hd);
	return 0;
}

*/



#if 0
int test_gpio_pinDebonceWrite(char *args)
{
    test_gpio_args_t td;
    gpio_pin_handle_t hd;

    int ret;
    gpio_event_cb_t cb_event;

    uint32_t get_data[4];
    //解析命令
    ret = args_parsing(args, get_data, 4);
    TEST_CASE_ASSERT_QUIT(ret == 0, "the number of parameters passed in does not match");

//赋值
    td.gpio_idx = (uint8_t)_dev_idx;
    td.dir = GPIO_DIRECTION_OUTPUT;
	td.pin_mask = (uint8_t)*((uint32_t *)args);
	td.gpio_mode = *((uint32_t *)args+1);
	td.pin_value  = (uint8_t)*((uint32_t *)args+2);
	td.delay_ms = (uint8_t)*((uint32_t *)args+3);

//打印一下
    TEST_CASE_TIPS("test GPIO idx is %d", td.gpio_idx);
    TEST_CASE_TIPS("config pins is 0x%x", td.pin_mask);
    TEST_CASE_TIPS("config direction is %d", td.dir); 
    TEST_CASE_TIPS("config mode is %d", td.gpio_mode);
    TEST_CASE_TIPS("expected write value is %d", td.pin_value);
	TEST_CASE_TIPS("output delay %d ms", td.delay_ms);

    hd = csi_gpio_pin_initialize(td.pin_mask,NULL);
    TEST_CASE_ASSERT_QUIT(hd != NULL, "gpio %d init fail.\n", td.gpio_idx);
    
    ret = csi_gpio_pin_config_direction(hd,td.dir);
    TEST_CASE_ASSERT_QUIT(ret == 0, "gpio direction config error: expected return value is 0,actual return value is %d.", ret);
    
    ret = csi_gpio_pin_config_mode(hd,td.gpio_mode);
    TEST_CASE_ASSERT_QUIT(ret == 0, "gpio mode config error: expected return value is 0,actual return value is %d.", ret);
    

    bool value = td.pin_value;
	ret = csi_gpio_pin_write(hd,value);
    TEST_CASE_ASSERT_QUIT(ret == 0, "gpio write error: expected return value is 0,actual return value is %d.", ret);
	
	tst_mdelay(50);

	TEST_CASE_READY();

	tst_mdelay(td.delay_ms);
    csi_gpio_pin_uninitialize(hd);
	return 0;
}


int test_gpio_pinsDebonceWrite(char *args);

#endif

