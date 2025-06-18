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
#if CONFIG_DEMO_PERF_RECORD
#include <csi_core.h>
#include <drv/tick.h>
#include <perf.h>
#include "app_init.h"

int test_func1(int val)
{
    int a = 100;

    for (int i = 0; i < val; i++) {
        a += i;
    }
    return a;
}

int test_func2(int val)
{
    int a = 0;

    for (int i = 0; i < val; i++) {
        a += test_func1(val);
    }
    return a;
}

int test_func3(int val)
{
    int a = 0;

    for (int i = 0; i < val; i++) {
        a += test_func2(val);
    }
    return a;
}

int test_func4(int val)
{
    int a = 0;

    for (int i = 0; i < val; i++) {
        a += test_func3(val);
    }
    return a;
}

int perf_record_test(void)
{
    int ret;
    int duration, frequency;

    duration = 5000;
    frequency = 999;

    perf_printf("perf record test start.[duration: %dms, frequency: %dHz]\n", duration, frequency);
    ret = perf_record_start(duration, frequency);
    if (ret != 0) {
        perf_printf("perf record start failed\n");
        return ret;
    }
    ret = test_func4(20);
    perf_printf("ret:%d\n", ret);
    mdelay(500);
    ret = test_func3(50);
    perf_printf("ret:%d\n", ret);
    mdelay(500);
    ret = test_func2(100);
    perf_printf("ret:%d\n", ret);
    mdelay(3000);
    perf_record_stop();
    perf_printf("perf record test finish.\n");
    return ret;
}
#endif // CONFIG_DEMO_PERF_RECORD