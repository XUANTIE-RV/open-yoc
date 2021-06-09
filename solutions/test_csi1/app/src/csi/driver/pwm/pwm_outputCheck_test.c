/*
 * Copyright (C) 2017 C-SKY Microsystems Co., All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "pwm_test.h"



static void gpio_event_cb(int idx)
{
	(void)idx;
}

int test_pwm_outputCheck(char *args)
{

    int32_t ret;
	uint32_t get_data[10];
    test_pwm_args_t td;
    gpio_pin_handle_t pin = NULL;
    volatile bool value = false;
	volatile uint32_t high_cnt = 0;
	volatile uint32_t low_cnt = 0;
	uint32_t period,pulse_width;

	/*args get*/
    ret = args_parsing(args, get_data, 3);
    TEST_CASE_ASSERT_QUIT(ret == 0, "the number of parameters passed in does not match");

    td.pin = (uint8_t)get_data[0];
    td.period = (uint32_t)get_data[1];
	td.pulse_width = (uint32_t)get_data[2];

    TEST_CASE_TIPS("config  PWM output check pin is %d", td.pin);
	TEST_CASE_TIPS("config period is %d us", td.period);
    TEST_CASE_TIPS("config pulse width is %d us", td.pulse_width);


	/*pinmux config*/
    /*ret = drv_pinmux_config(td.pin, PIN_FUNC_GPIO);
    TEST_CASE_ASSERT(ret == 0,"gpio pin mux failed");*/

	/*gpio pin init*/
    pin = csi_gpio_pin_initialize(td.pin, gpio_event_cb);
    TEST_CASE_ASSERT(pin != NULL,"gpio pin init failed");

	/*gpio pin config mode*/
    ret = csi_gpio_pin_config_mode(pin, GPIO_MODE_PULLUP);
    TEST_CASE_ASSERT(ret == 0,"gpio pin config mode failed");

	/*gpio pin config direction*/
    ret = csi_gpio_pin_config_direction(pin, GPIO_DIRECTION_INPUT);
    TEST_CASE_ASSERT(ret == 0,"gpio pin config direction failed");

	/*gpio pin config irq*/
    ret = csi_gpio_pin_set_irq(pin, GPIO_IRQ_MODE_RISING_EDGE, 1);
    TEST_CASE_ASSERT(ret == 0,"gpio pin config irq failed");


	TEST_CASE_READY();

	value = true;
	while(value == true)
	{
		ret = csi_gpio_pin_read(pin, &value);
    	TEST_CASE_ASSERT_QUIT(ret == 0,"gpio pin read failed");
	}

	value = false;
	while(value == false)
	{
		ret = csi_gpio_pin_read(pin, &value);
    	TEST_CASE_ASSERT_QUIT(ret == 0,"gpio pin read failed");
	}

	value = true;
	while(value == true)
	{
		tst_mdelay(1);
		ret = csi_gpio_pin_read(pin, &value);
    	TEST_CASE_ASSERT_QUIT(ret == 0,"gpio pin read failed");
		high_cnt++;
	}

	value = false;
	while(value == false)
	{
		tst_mdelay(1);
		ret = csi_gpio_pin_read(pin, &value);
    	TEST_CASE_ASSERT_QUIT(ret == 0,"gpio pin read failed");
		low_cnt++;
	}

    TEST_CASE_TIPS("pwm high level cnt is %d, low level cnt is %d", high_cnt,low_cnt);
	period = low_cnt + high_cnt;
	pulse_width = low_cnt;

	if (((period-2) > td.period/1000) || ((period+2) < td.period/1000))
		TEST_CASE_ASSERT(1 == 0, "the test of period value is %d, set value is %d",period , td.period/1000);

	if (((pulse_width-2) > td.pulse_width/1000) || ((pulse_width+1) < td.pulse_width/1000))
		TEST_CASE_ASSERT(1 == 0, "the test of pulse_width value is %d, set value is %d",pulse_width , td.pulse_width/1000);

	csi_gpio_pin_uninitialize(pin);

	return 0;

}