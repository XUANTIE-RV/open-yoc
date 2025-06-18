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
#if CONFIG_DEMO_PERF_STAT
#include <stdio.h>
#include <stdarg.h>
#include <csi_core.h>
#include <drv/tick.h>
#include <perf.h>
#include <string.h>
#include <app_init.h>

void perf_stat_test(void)
{
    perf_printf("start, delaying for seconds, please wait...\r\n");
    uint16_t evs[] = {
        PMU_HPM_L1_icache_Access,
        PMU_HPM_L1_icache_Miss,
        PMU_HPM_iTLB_Miss,
        PMU_HPM_jTLB_Miss,
        PMU_HPM_Condition_Branch_Mispred,
        PMU_HPM_Condition_Branch,
        PMU_HPM_Indirect_Branch_Miss,
        PMU_HPM_Indirect_Branch,
        PMU_HPM_Store_Instruction,
        PMU_HPM_L1_dcache_load_access,
    };
    perf_stat_args_t args = {
        .events = evs,
        .nr_events = sizeof(evs) / sizeof(uint16_t),
    };
    perf_stat_start(&args);
    // perf_stat_start(NULL); // use default
    mdelay(3000);
    perf_printf("stop\r\n");
    perf_stat_stop();
}

#endif // CONFIG_DEMO_PERF_STAT