
#include <gpio_test.h>

int test_gpio_pinRead(char *args)
{

    test_gpio_args_t td;
    gpio_pin_handle_t hd;
    uint32_t expect_value,actual_value,read_value;

    int ret;
    bool volatile temp;

//赋值
    td.dir = GPIO_DIRECTION_INPUT;
	td.pin = (uint8_t)*((uint32_t *)args);
	td.gpio_mode =*((uint32_t *)args+1);
	td.pin_value  = (uint8_t)*((uint32_t *)args+2);

//打印一下
    TEST_CASE_TIPS("config pins is 0x%x", td.pin);
    TEST_CASE_TIPS("config direction is %d", td.dir); 
    TEST_CASE_TIPS("config mode is %d", td.gpio_mode);
    TEST_CASE_TIPS("expected write value is %d", td.pin_value);

    hd = csi_gpio_pin_initialize(td.pin,NULL);
    TEST_CASE_ASSERT_QUIT(hd != NULL, "gpio init fail.\n");
    
    ret = csi_gpio_pin_config_direction(hd,td.dir);
    TEST_CASE_ASSERT_QUIT(ret == 0, "gpio direction config error: expected return value is 0,actual return value is %d.", ret);
    
    ret = csi_gpio_pin_config_mode(hd,td.gpio_mode);
    TEST_CASE_ASSERT_QUIT(ret == 0, "gpio mode config error: expected return value is 0,actual return value is %d.", ret);
    
    expect_value = (uint32_t)(td.pin_value);
    TEST_CASE_TIPS("expect_value is %d",expect_value);
    TEST_CASE_READY();

    ret = csi_gpio_pin_read(hd,&temp);
    TEST_CASE_ASSERT_QUIT(ret == 0, "gpio read error");

    read_value = (uint8_t)temp;
    TEST_CASE_TIPS("read_value is %d",read_value);
//    actual_value = (read_value & (1<<td.pin))>> td.pin;
    TEST_CASE_ASSERT_QUIT(expect_value == read_value, "gpio read error: expect_value is %d,actual_value is %d.", expect_value,read_value);

/*
    temp = td.pin_value;

	read_value = csi_gpio_pin_read(hd,&temp);
    TEST_CASE_TIPS("read_value is %d",read_value);
    actual_value = (read_value & (1<<td.pin))>> td.pin;

    TEST_CASE_ASSERT_QUIT(expect_value == actual_value, "gpio read error: expect_value is %d,actual_value is %d.", expect_value,actual_value);
*/
    csi_gpio_pin_uninitialize(hd);
	return 0;
}



#if 0
int test_gpio_pinsRead(char *args)
{

    test_gpio_args_t td;
    gpio_pin_handle_t hd;
    uint32_t expect_value,actual_value,read_value;

    int ret;
    bool temp;

//赋值
    td.dir = GPIO_DIRECTION_INPUT;
	td.pin_mask = (uint8_t)*((uint32_t *)args);
	td.gpio_mode =*((uint32_t *)args+1);
	td.pin_value  = (uint8_t)*((uint32_t *)args+2);

//打印一下
    TEST_CASE_TIPS("config pins is 0x%x", td.pin_mask);
    TEST_CASE_TIPS("config direction is %d", td.dir); 
    TEST_CASE_TIPS("config mode is %d", td.gpio_mode);
    TEST_CASE_TIPS("expected write value is %d", td.pin_value);


    hd = csi_gpio_pin_initialize(td.pin_mask,NULL);
    TEST_CASE_ASSERT_QUIT(hd != NULL, "gpio  init fail.\n");
    
    ret = csi_gpio_pin_config_direction(hd,td.dir);
    TEST_CASE_ASSERT_QUIT(ret == 0, "gpio direction config error: expected return value is 0,actual return value is %d.", ret);
    
    ret = csi_gpio_pin_config_mode(hd,td.gpio_mode);
    TEST_CASE_ASSERT_QUIT(ret == 0, "gpio mode config error: expected return value is 0,actual return value is %d.", ret);
    
    expect_value = (uint32_t)(td.pin_value);
    TEST_CASE_TIPS("expect_value is %d",expect_value);
    TEST_CASE_READY();

	ret = csi_gpio_pin_read(hd,&temp);
    TEST_CASE_ASSERT_QUIT(ret == 0, "gpio read error");

    read_value = (uint8_t)temp;

    TEST_CASE_TIPS("read_value is %d",read_value);
//    actual_value = temp & td.pin_mask;
    
    TEST_CASE_ASSERT_QUIT(expect_value == read_value, "gpio read error: expect_value is %d,actual_value is %d.", expect_value,read_value);

    csi_gpio_pin_uninitialize(hd);
	return 0;
}
#endif
#if 0
int test_gpio_debonce_pinRead(char *args);
int test_gpio_debonce_pinsRead(char *args);
int test_gpio_pinsToggleRead(char *args);
int test_gpio_deboncePinsToggleRead(char *args);
#endif