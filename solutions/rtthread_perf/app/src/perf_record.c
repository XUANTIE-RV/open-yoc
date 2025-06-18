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
#include <csi_core.h>
#include <drv/tick.h>
#include <perf.h>
#include <rtthread.h>
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

int perf_record_test(int duration, int frequency)
{
    int ret, ret1, ret2, ret3;

    perf_printf("perf record test start.[duration: %dms, frequency: %d]\n", duration, frequency);
    ret = perf_record_start(duration, frequency);
    if (ret != 0) {
        perf_printf("perf record start failed\n");
        return ret;
    }
    ret1 = test_func4(20);
    rt_thread_mdelay(500);
    ret2 = test_func3(50);
    rt_thread_mdelay(500);
    ret3 = test_func2(100);
    perf_printf("perf record test func val.[%d]\n", ret1 + ret2 + ret3);
    return ret;
}