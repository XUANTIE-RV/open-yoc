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

#ifndef HAL_GNSS_IMPL_H
#define HAL_GNSS_IMPL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <devices/gnss.h>

typedef struct {
    driver_t drv;
    int (*get_info)(rvm_dev_t *dev, rvm_hal_gnss_info_t *info);

} gnss_driver_t;

#ifdef __cplusplus
}
#endif

#endif
