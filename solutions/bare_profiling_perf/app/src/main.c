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

#include <stdio.h>
#include <perf.h>
#include "board.h"
#include "app_init.h"

int main(void)
{
    board_init();

    perf_stat_init();
    perf_record_init();

    printf("bare_profiling_perf start.\n");
#if CONFIG_SMP
    extern void perf_spinlock_init(void);
    perf_spinlock_init();
    secondary_cpu_up();
#endif /* CONFIG_SMP */

#if CONFIG_DEMO_PERF_STAT
    extern int perf_stat_test(void);
    perf_stat_test();
#endif

#if CONFIG_DEMO_PERF_RECORD
    extern int perf_record_test(void);
    perf_record_test();
#endif

    perf_stat_deinit();
    perf_record_deinit();
    return 0;
}

