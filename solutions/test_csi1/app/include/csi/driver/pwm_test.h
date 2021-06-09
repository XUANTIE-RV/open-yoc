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

#ifndef __PWM_TEST__
#define __PWM_TEST__

#include <stdint.h>
#include <drv/pwm.h>
#include <drv/gpio.h>
#include <soc.h>
#include <string.h>
#include <stdlib.h>
#include <autotest.h>
#include <test_log.h>
#include <test_config.h>
#include <pinmux_test.h>


typedef struct {
    uint32_t    dev_idx;
    uint8_t     channel;
    uint32_t    period;
    uint32_t    pulse_width;
    uint8_t     output_polarity;
	uint8_t		capture_polarity;
	uint32_t	capture_count;
	uint8_t		pin;
	uint32_t	delay_ms;
}test_pwm_args_t;




extern int test_pwm_interface(char *args);
extern int test_pwm_output(char *args);
extern int test_pwm_outputCheck(char *args);



#endif
