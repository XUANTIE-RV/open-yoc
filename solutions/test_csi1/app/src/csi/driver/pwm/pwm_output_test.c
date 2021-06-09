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


int test_pwm_output(char *args)
{

    int32_t ret;
	uint32_t get_data[10];
    test_pwm_args_t td;

	/*args get*/
    ret = args_parsing(args, get_data, 5);
    TEST_CASE_ASSERT_QUIT(ret == 0, "the number of parameters passed in does not match");

	td.dev_idx = (uint8_t)get_data[0];
	td.channel = (uint8_t)get_data[1];
	td.period = (uint32_t)get_data[2];
	td.pulse_width = (uint32_t)get_data[3];
	td.delay_ms = (uint32_t)get_data[4];

	TEST_CASE_TIPS("test PWM idx is %d", td.dev_idx);
    TEST_CASE_TIPS("test pwm channel is %d", td.channel);
	TEST_CASE_TIPS("config period is %d us", td.period);
	TEST_CASE_TIPS("config pulse width is %d us", td.pulse_width);
	TEST_CASE_TIPS("config output time is %d ms", td.delay_ms);

    /*pwm init*/
    pwm_handle_t pwm = csi_pwm_initialize(td.dev_idx);
    TEST_CASE_ASSERT(pwm != NULL,"pwm init failed");

    /*pwm config*/
    ret = csi_pwm_config(pwm, td.channel, td.period, td.pulse_width);
    TEST_CASE_ASSERT(ret == 0,"pwm config failed");

	TEST_CASE_READY();

	csi_pwm_start(pwm, td.channel);

	tst_mdelay(td.delay_ms);

    /*pwm uninit*/
	csi_pwm_stop(pwm, td.channel);
	csi_pwm_uninitialize(pwm);

	return 0;    

}