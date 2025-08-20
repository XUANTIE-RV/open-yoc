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

#ifndef __AOS_WDT_H__
#define __AOS_WDT_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

long aos_wdt_index();
void aos_wdt_debug(int en);
int aos_wdt_hw_enable(int id, int ms);
void aos_wdt_hw_disable(int id);
void aos_wdt_attach(long index, void (*will)(void *), void *args);
void aos_wdt_detach(long index);
int  aos_wdt_exists(long index);
void aos_wdt_feed(long index, int max_time);
void aos_wdt_show(long index);
void aos_wdt_showall();

#ifdef __cplusplus
}
#endif

#endif
