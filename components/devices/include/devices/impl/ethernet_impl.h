 /*
 * Copyright (C) 2017-2024 Alibaba Group Holding Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _DRIVERS_ETHDRV_IMPL_H_
#define _DRIVERS_ETHDRV_IMPL_H_

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include <aos/aos.h>

#ifdef __cplusplus
extern "C" {
#endif
typedef struct eth_driver {
    /*common*/
    int (*mac_control)(rvm_dev_t *dev, eth_config_t *config);
    int (*set_packet_filter)(rvm_dev_t *dev, int type);
    int (*start)(rvm_dev_t *dev);
    int (*stop)(rvm_dev_t *dev);
    int (*reset)(rvm_dev_t *dev);
} eth_driver_t;

#ifdef __cplusplus
}
#endif

#endif
