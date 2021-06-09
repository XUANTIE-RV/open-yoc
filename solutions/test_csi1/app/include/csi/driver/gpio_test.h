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

#ifndef __GPIO_TEST__
#define __GPIO_TEST__

#include <stdint.h>
#include <drv/gpio.h>
#include <soc.h>
#include <string.h>
#include <stdlib.h>
#include <autotest.h>
#include <test_log.h>
#include <test_config.h>
#include <test_common.h>

typedef struct {
    uint8_t     gpio_idx;
    uint32_t    pin_mask;
    uint8_t     dir;
    uint8_t     gpio_mode;
    uint8_t     gpio_irq_mode;
	uint32_t	delay_ms;
	uint8_t		pin;
	bool		pin_value;
}test_gpio_args_t;


extern int test_gpio_interface(char *args);
//extern int test_gpio_pinsWrite(char *args);
extern int test_gpio_pinWrite(char *args);
extern int test_gpio_pinRead(char *args);
//extern int test_gpio_pinsRead(char *args);
extern int test_gpio_interrupt_Capture(char *args);
extern int test_gpio_interrupt_Trigger(char *args);
extern int test_gpio_interface(char *args);
#endif