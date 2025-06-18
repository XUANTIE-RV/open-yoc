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

#ifndef __PERF_TYPES_H__
#define __PERF_TYPES_H__

#include <drv/timer.h>

#ifdef __cplusplus
extern "C" {
#endif

#define RECORD_STATE_BUF_FULL 			1
#define RECORD_STATE_TRACE_FINISH 		2
#define RECORD_STATE_TRACING 			3
#define RECORD_STATE_FORCE_STOP 		4
#define RECORD_STATE_BUF_DUMP 			5
#define RECORD_STATE_BUF_DUMP_END 		6

typedef enum {
    PERF_OK = 0,
    PERF_ERROR = 1,
} perf_err_t;

typedef void *perf_thread_id;

// arguments of perf_stat_start()
typedef struct perf_stat_args_st {
    uint16_t* events;
    uint8_t *cpu_ids;
    perf_thread_id* tids;
    uint32_t nr_threads;
    uint16_t nr_events;
    uint8_t nr_cpus;
    uint8_t pmu_schedule;
} perf_stat_args_t;

typedef struct {
    uint16_t cpu_id;
    uint16_t timer_idx;
} cpu_bind_timer_t;

typedef struct perf_data {
	uint32_t tick;
	uint32_t duration_ms;
	uint32_t frequency;
	volatile uint32_t state;
	size_t buf_idx;
	unsigned long *buf;
	csi_timer_t timer;
} perf_data_t;


#ifdef __cplusplus
}
#endif

#endif /* __PERF_TYPES_H__ */
