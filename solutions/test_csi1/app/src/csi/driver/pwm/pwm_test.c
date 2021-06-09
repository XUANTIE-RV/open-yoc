/*
 * Copyright (C) 2020 C-SKY Microsystems Co., Ltd. All rights reserved.
 * 
 * Licensed under the Apache License, Version 2.0 (the 'License');
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an 'AS IS' BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions an
 * limitations under the License.
 */

#include <pwm_test.h>


int test_pwm_main(char *args)
{	
	char *case_name[] = {
		"PWM_INTERFACE",
		"PWM_OUTPUT",
    	"PWM_OUTPUT_CHECK",

	};

	int (*case_func[])(char *args) = {
		test_pwm_interface,
		test_pwm_output,
    	test_pwm_outputCheck,

	};



	uint8_t i = 0;

    for (i=0; i<sizeof(case_name)/sizeof(char *); i++) {
        if (!strcmp((void *)_mc_name, case_name[i])) {
            case_func[i](args);
            return 0;
        }
    }

    TEST_CASE_TIPS("PWM don't supported this command");
    return -1;

}
