
#include <gpio_test.h>

static volatile uint32_t irq_pin;
static volatile uint8_t cb_irq;
static volatile int flag = 0;
static volatile uint8_t lock = 0;
static test_gpio_args_t gpio_td; 
static gpio_pin_handle_t hd;



static void gpio_callback(int32_t idx)
{
    lock++;
    int32_t ret;
	if (gpio_td.gpio_irq_mode != GPIO_IRQ_MODE_DOUBLE_EDGE) {
        ret = csi_gpio_pin_set_irq(hd, cb_irq, 0);
        TEST_CASE_ASSERT_QUIT(ret == 0, "gpio_pin_set_irq fail.\n");
	}	
}


int test_gpio_interrupt_Capture(char *args)
{
    test_gpio_args_t td;
    uint32_t expect_value,actual_value,read_value;

    int ret;

    td.dir = GPIO_DIRECTION_INPUT;
	td.pin = (uint8_t)*((uint32_t *)args);
	td.gpio_mode = (uint8_t)*((uint32_t *)args+1);
	td.gpio_irq_mode = (uint8_t)*((uint32_t *)args+2);
    cb_irq = td.gpio_irq_mode;
//打印一下
    TEST_CASE_TIPS("config pins is 0x%x", td.pin);
    TEST_CASE_TIPS("config direction is %d", td.dir); 
    TEST_CASE_TIPS("config mode is %d", td.gpio_mode);
    TEST_CASE_TIPS("config mode is %d", td.gpio_irq_mode);


    hd = csi_gpio_pin_initialize(td.pin,gpio_callback);
    TEST_CASE_ASSERT_QUIT(hd != NULL, "gpio %d init fail.\n");
    
    ret = csi_gpio_pin_config_direction(hd,td.dir);
    TEST_CASE_ASSERT_QUIT(ret == 0, "gpio direction config error: expected return value is 0,actual return value is %d.", ret);
    
    ret = csi_gpio_pin_config_mode(hd,td.gpio_mode);
    TEST_CASE_ASSERT_QUIT(ret == 0, "gpio mode config error: expected return value is 0,actual return value is %d.", ret);
    
    ret = csi_gpio_pin_set_irq(hd,td.gpio_irq_mode,true);
    TEST_CASE_ASSERT_QUIT(ret == 0,"gpio enable irq error.\n");

    lock = 0;
    
    TEST_CASE_READY();
    if(td.gpio_irq_mode == GPIO_IRQ_MODE_DOUBLE_EDGE){
        while(lock != 2);
    }
    else{
        while(lock != 1);
    }

    ret = csi_gpio_pin_set_irq(hd,td.gpio_irq_mode,false);
    TEST_CASE_ASSERT_QUIT(ret == 0,"gpio enable irq error.\n");

    csi_gpio_pin_uninitialize(hd);
	return 0;
}



int test_gpio_interrupt_Trigger(char *args)
{
    uint32_t read_value;

    int ret;

//赋值
//    td.gpio_idx = (uint8_t)_dev_idx;
    gpio_td.dir = GPIO_DIRECTION_OUTPUT;
	gpio_td.pin = (uint8_t)*((uint32_t *)args);
	gpio_td.gpio_mode = (uint8_t)*((uint32_t *)args+1);
	gpio_td.gpio_irq_mode = (uint8_t)*((uint32_t *)args+2);
    gpio_td.delay_ms = (uint8_t)*((uint32_t *)args+3);

//打印一下

    TEST_CASE_TIPS("config pins is 0x%x", gpio_td.pin);
    TEST_CASE_TIPS("config direction is %d", gpio_td.dir); 
    TEST_CASE_TIPS("config mode is %d", gpio_td.gpio_mode);
    TEST_CASE_TIPS("config trigger mode is %d", gpio_td.gpio_irq_mode);
	TEST_CASE_TIPS("config trigger delay is %d ms", gpio_td.delay_ms);

    hd = csi_gpio_pin_initialize(gpio_td.pin,NULL);
    TEST_CASE_ASSERT_QUIT(hd != NULL, "gpio %d init fail.\n", gpio_td.gpio_idx);
    
    ret = csi_gpio_pin_config_direction(hd,gpio_td.dir);
    TEST_CASE_ASSERT_QUIT(ret == 0, "gpio direction config error: expected return value is 0,actual return value is %d.", ret);
    
    ret = csi_gpio_pin_config_mode(hd,gpio_td.gpio_mode);
    TEST_CASE_ASSERT_QUIT(ret == 0, "gpio mode config error: expected return value is 0,actual return value is %d.", ret);

    switch(gpio_td.gpio_irq_mode){
        case GPIO_IRQ_MODE_RISING_EDGE:
            csi_gpio_pin_write(hd,false);
            TEST_CASE_READY();
            tst_mdelay(gpio_td.delay_ms);
            csi_gpio_pin_write(hd,true);
            break;
        case GPIO_IRQ_MODE_FALLING_EDGE:
            csi_gpio_pin_write(hd,true);
            TEST_CASE_READY();
            tst_mdelay(gpio_td.delay_ms);
            csi_gpio_pin_write(hd,false);
            break;
        case GPIO_IRQ_MODE_DOUBLE_EDGE:
            csi_gpio_pin_write(hd,false);
            TEST_CASE_READY();
            tst_mdelay(gpio_td.delay_ms);
            csi_gpio_pin_write(hd,true);
            break;
        case GPIO_IRQ_MODE_LOW_LEVEL:
            csi_gpio_pin_write(hd,true);
            TEST_CASE_READY();
            tst_mdelay(gpio_td.delay_ms);
            csi_gpio_pin_write(hd,false);
            break;
        case GPIO_IRQ_MODE_HIGH_LEVEL:
            csi_gpio_pin_write(hd,false);
            TEST_CASE_READY();
            tst_mdelay(gpio_td.delay_ms);
            csi_gpio_pin_write(hd,true);
            break;
        default:
            break;
    }

    tst_mdelay(100);
    csi_gpio_pin_uninitialize(hd);
	return 0;
}
