/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ctype.h>
#include <aos/kernel.h>
#include <aos/aos.h>

#include <aos/hal/gpio.h>
#include <soc.h>
// #include <ulog/ulog.h>
#include <yoc/atserver.h>

#define GPIO_LED_IO PA24
#define GPIO_BUTTON_IO PA23
#define GPIO_BUTTON_IO_2 PA29
gpio_dev_t led, read1;
gpio_dev_t button1;
int entry_flag = 0;
int timeout_flag = 0;

void should_be_fail(int ret)
{
    if (ret==0) {
        AT_BACK_ERR();
    }
}
void should_be_pass(int ret)
{
    if (ret!=0) {
        AT_BACK_ERR();
    }
}

void button1_handler(void *arg)
{
    entry_flag = 1;
	printf("enter button fun\n");
}

void hal_gpio_api_test(void)
{
    uint32_t value = 0;
    printf("start test\n");
    int ret = -1;

    /* gpio port config */
    led.port = GPIO_LED_IO;

    /* set as output mode */
    led.config = OUTPUT_PUSH_PULL;

    /* configure GPIO with the given settings */
    ret = hal_gpio_init(NULL);
    should_be_fail(ret);
   
    ret = hal_gpio_init(&led);
    should_be_pass(ret);

    ret = hal_gpio_output_low(NULL);
    should_be_fail(ret);

    ret = hal_gpio_output_low(&led);
    should_be_pass(ret);

    ret = hal_gpio_output_high(&led);
    should_be_pass(ret);

    ret = hal_gpio_input_get(NULL,NULL);
    should_be_fail(ret);

    ret = hal_gpio_finalize(&led);
    should_be_pass(ret);

    ret = hal_gpio_finalize(NULL);
    should_be_fail(ret);

    ret = hal_gpio_enable_irq(NULL, IRQ_TRIGGER_FALLING_EDGE, 
                              button1_handler, NULL);
    should_be_fail(ret);

    AT_BACK_OK();
}

void hal_gpio_set_read(gpio_config_t led_config, gpio_config_t read_config, uint8_t led_port, uint8_t read_port)
{
    uint32_t value = 2;
    printf("start test\n");
    int ret = -1;

    /* gpio port config */
    led.port = led_port;

    /* set as output mode */
    led.config = led_config;

    /* configure GPIO with the given settings */
    printf("led init\n");
    ret = hal_gpio_init(&led);
    should_be_pass(ret);

    /* gpio port config */
    read1.port = read_port;

    /* set as output mode */
    read1.config = read_config;

    printf("read1 init\n");
    ret = hal_gpio_init(&read1);
    should_be_pass(ret);

    /* output low */
    printf("low\n");
    ret = hal_gpio_output_low(&led);
    should_be_pass(ret);
    aos_msleep(1000);
    ret = hal_gpio_input_get(&read1,&value);
    should_be_pass(ret);
    if (value != 0) {
        hal_gpio_finalize(&led);
        hal_gpio_finalize(&read1);
        AT_BACK_ERR();
        return;
    }

    /* output high */
    printf("high\n");
    ret = hal_gpio_output_high(&led);
    should_be_pass(ret);
    aos_msleep(1000);
    ret = hal_gpio_input_get(&read1,&value);
    should_be_pass(ret);
    if (value != 1) {
        printf("get high error\n");
        hal_gpio_finalize(&led);
        hal_gpio_finalize(&read1);
        AT_BACK_ERR();
        return;
    } 

    /* output toggle */
    printf("toggle\n");
    ret = hal_gpio_output_toggle(&led);
    should_be_pass(ret);
    aos_msleep(1000);
    ret = hal_gpio_input_get(&read1,&value);
    should_be_pass(ret);
    if (value != 0) {
        printf("get toggle1 error\n");
        hal_gpio_finalize(&led);
        hal_gpio_finalize(&read1);
        AT_BACK_ERR();
        return;
    }

    printf("toggle\n");
    ret = hal_gpio_output_toggle(&led);
    should_be_pass(ret);
    aos_msleep(1000);
    ret = hal_gpio_input_get(&read1,&value);
    should_be_pass(ret);
    if (value != 1) {
        printf("get toggle2 error\n");
        hal_gpio_finalize(&led);
        hal_gpio_finalize(&read1);
        AT_BACK_ERR();
        return;
    }
    hal_gpio_finalize(&led);
    hal_gpio_finalize(&read1);
    AT_BACK_OK();	
}

void TASK_contrl_gpio_test(void *arg)
{
    printf("start test\n");
    int ret = -1;

    /* gpio port config */
    led.port = GPIO_LED_IO;

    /* set as output mode */
    led.config = OUTPUT_PUSH_PULL;

    /* configure GPIO with the given settings */
    printf("led init\n");
    ret = hal_gpio_init(&led);
    should_be_pass(ret);
    
    /* output low */
    printf("output contrl low\n");
    hal_gpio_output_low(&led);
    aos_msleep(3000);
    printf("output contrl high\n");
    hal_gpio_output_high(&led);
    aos_msleep(3000);
    printf("output contrl low\n");
    hal_gpio_output_low(&led);
    timeout_flag = 1;
}

void hal_gpio_irq_test(int trigger)
{
    timeout_flag = 0;
    entry_flag = 0;
    int ret = 0;
    /* input pin config */
	button1.port = GPIO_BUTTON_IO;

	/* set as interrupt mode */
	button1.config = IRQ_MODE;
    
	/* configure GPIO with the given settings */
	printf("button1 init\n");
    ret = hal_gpio_init(&button1);
	should_be_pass(ret);
    button1.config = INPUT_PULL_UP;
    ret = hal_gpio_init(&button1);
	if (ret != 0) {
	    printf("gpio init error !\n");
        AT_BACK_ERR();
        return;
	}

	/* gpio interrupt config */
	ret = hal_gpio_enable_irq(&button1, trigger, 
                              button1_handler, NULL);
    if (ret != 0) {
        printf("gpio irq enable error !\n");
	}
    ret = aos_task_new("contrl_gpio",
                           TASK_contrl_gpio_test, NULL, 1024);
    printf("timeout_flag=%d\n",timeout_flag);
    while(1) {
        printf("entry_flag=%d\n",entry_flag);
        if (entry_flag == 1) {
            ret = hal_gpio_disable_irq(&button1);
            should_be_pass(ret);
            ret = hal_gpio_clear_irq(&button1);
            should_be_pass(ret);
            AT_BACK_OK(); 
            entry_flag = 0;
            return;
        } else if (timeout_flag == 1) {
            AT_BACK_ERR(); 
            hal_gpio_disable_irq(&button1);
            hal_gpio_clear_irq(&button1);
            return;
        }
    }
    hal_gpio_finalize(&button1);
}

void test_hal_gpio(char *cmd, int type, char *data)
{
    csi_pin_set_mux(PA24,4);
    csi_pin_set_mux(PA23,4);
    csi_pin_set_mux(PA29,4);
    if (strcmp((const char *)data, "'API'\0") == 0) {
        hal_gpio_api_test();
    } else if (strcmp((const char *)data, "'SET1'\0") == 0) {
        hal_gpio_set_read(OUTPUT_PUSH_PULL, INPUT_PULL_UP, GPIO_LED_IO, GPIO_BUTTON_IO);
    } else if (strcmp((const char *)data, "'SET2'\0") == 0) {
        hal_gpio_set_read(OUTPUT_PUSH_PULL, INPUT_PULL_DOWN, GPIO_LED_IO, GPIO_BUTTON_IO_2);
    } else if (strcmp((const char *)data, "'SET3'\0") == 0) {
        hal_gpio_set_read(OUTPUT_PUSH_PULL, INPUT_HIGH_IMPEDANCE, GPIO_LED_IO, GPIO_BUTTON_IO);
    } else if (strcmp((const char *)data, "'SET4'\0") == 0) {
        hal_gpio_set_read(OUTPUT_OPEN_DRAIN_NO_PULL, INPUT_PULL_UP, GPIO_LED_IO, GPIO_BUTTON_IO);
    } else if (strcmp((const char *)data, "'SET5'\0") == 0) {
        hal_gpio_set_read(OUTPUT_OPEN_DRAIN_PULL_UP, INPUT_PULL_UP, GPIO_LED_IO, GPIO_BUTTON_IO);
    } else if (strcmp((const char *)data, "'IRQ_FALLING'\0") == 0) {
        hal_gpio_irq_test(IRQ_TRIGGER_FALLING_EDGE);
    } else if (strcmp((const char *)data, "'IRQ_RISING'\0") == 0) {
        hal_gpio_irq_test(IRQ_TRIGGER_RISING_EDGE);
    } else if (strcmp((const char *)data, "'IRQ_BOTH'\0") == 0) {
        hal_gpio_irq_test(IRQ_TRIGGER_BOTH_EDGES);
    }
}