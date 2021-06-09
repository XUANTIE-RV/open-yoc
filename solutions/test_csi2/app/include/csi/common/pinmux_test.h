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

#ifndef __PINMUX_TEST__
#define __PINMUX_TEST__

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <test_config.h>
#include <test_log.h>
#include <test_common.h>

typedef struct {
	uint8_t     dev_idx;
    uint8_t     pin;
    uint8_t     pin_func;
}test_pinmux_args_t;


extern int test_pinmux_config(test_pinmux_args_t *pins);


#endif
