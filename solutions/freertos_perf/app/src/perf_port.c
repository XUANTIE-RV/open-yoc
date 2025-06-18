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
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <csi_core.h>
#include <drv/tick.h>
#include <perf.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include "app_init.h"

#if CONFIG_NR_CPUS > 1
uint8_t is_in_interrupt()
{
    return g_irq_nested_level[portGET_CORE_ID()] != 0;
}
#else
uint8_t is_in_interrupt()
{
    return g_irq_nested_level != 0;
}
#endif

cpu_bind_timer_t g_cpu_bind_timer[8] = {
	{0, 0},
	{1, 1},
	{2, 2},
	{3, 3}
};


void perf_spinlock_init()
{
}

int perf_printf(const char *fmt, ...)
{
    int ret;
    va_list args;

    if (is_in_interrupt())
    {
        unsigned long isr_status = taskENTER_CRITICAL_FROM_ISR();
        printf("cpuid[%d] ", portGET_CORE_ID());
        va_start(args, fmt);
        ret = vprintf(fmt, args);
        taskEXIT_CRITICAL_FROM_ISR(isr_status);
    }
    else
    {
        taskENTER_CRITICAL();
        printf("cpuid[%d] ", portGET_CORE_ID());
        va_start(args, fmt);
        ret = vprintf(fmt, args);
        taskEXIT_CRITICAL();
    }
    return ret;
}

void *perf_malloc(size_t size)
{
    portASSERT_IF_IN_ISR();
    unsigned long *p = (unsigned long *)malloc(size);
    configASSERT(p != NULL);
    return p;
}

void *perf_realloc(void *ptr, size_t size)
{
    portASSERT_IF_IN_ISR();
    unsigned long *p;
    p = (unsigned long *)realloc(ptr, size);
    return p;
}

void perf_free(void* ptr)
{
    portASSERT_IF_IN_ISR();
    if (ptr)
    {
        free(ptr);
    }
}

static unsigned long *perf_record_data_buf[configNUMBER_OF_CORES];
unsigned long *perf_record_alloc_data_buf(void)
{
    int cpuid = portGET_CORE_ID();
    if (perf_record_data_buf[cpuid] == NULL)
    {
        perf_record_data_buf[cpuid] = (unsigned long *)malloc(CONFIG_PROFILING_PERF_PER_CORE_BUFFER_SIZE);
    }
    configASSERT(perf_record_data_buf[cpuid] != NULL);
    memset((uint8_t*)perf_record_data_buf[cpuid], 0, CONFIG_PROFILING_PERF_PER_CORE_BUFFER_SIZE);
    return perf_record_data_buf[cpuid];
}

void perf_record_free_data_buf(unsigned long *buf)
{
    configASSERT(buf != NULL);
    if (buf) {
        buf = NULL;
    }
}

int perf_num_cpus_get(void)
{
#if CONFIG_SMP
    return CONFIG_NR_CPUS;
#else
	return 1;
#endif
}

size_t perf_record_per_core_buffer_size(void)
{
    return CONFIG_PROFILING_PERF_PER_CORE_BUFFER_SIZE;
}

#if CONFIG_PERF_DATA_SEMIHOST2_DUMP
#include <semihost2/semihost.h>
#ifndef SEMI_CHECK_RET_WITH_GOTO
#define SEMI_CHECK_RET_WITH_GOTO(x, label) \
	do { \
		if (!(x)) { \
			perf_printf("func[%s], line[%d] fail.\r\n", __FUNCTION__, __LINE__); \
			goto label; \
		}\
	} while (0)
#endif
#endif
#if CONFIG_PERF_DATA_GDB_MANUAL_DUMP
static volatile int g_dump_flag;
#endif
int perf_record_dump(void)
{
	int cpu_id = csi_get_cpu_id();
	int cpu_num = perf_num_cpus_get();

	if (g_perf_data[cpu_id].buf_idx == 0) {
		perf_printf("there is no data, do not need dump.\n");
		return 0;
	}
	g_perf_data[cpu_id].state = RECORD_STATE_BUF_DUMP;

#if defined(CONFIG_SMP) && CONFIG_SMP
	while(1) {
		int i;
		for (i = 0; i < cpu_num; i++) {
			if (g_perf_data[i].state != RECORD_STATE_BUF_DUMP && g_perf_data[i].buf_idx) {
				break;
			}
		}
		if (i == cpu_num)
			break;
		mdelay(20);
	}
	if (cpu_id != 0) {
		while(g_perf_data[cpu_id].state != RECORD_STATE_BUF_DUMP_END) {}
		perf_printf("no need dump.\n");
		return 0;
	}
#endif

#if CONFIG_PERF_DATA_GDB_MANUAL_DUMP
	perf_printf("please use riscv64-unknown-elf-gdb to dump data:\n");
	for (int i = 0; i < cpu_num; i++) {
		if (g_perf_data[i].buf_idx) {
			perf_printf("dump memory perf_rtos_%d.data 0x%lx 0x%lx+%ld \n", i,
						(unsigned long)g_perf_data[i].buf, (unsigned long)g_perf_data[i].buf, g_perf_data[i].buf_idx * sizeof(unsigned long));
		}
	}
	g_dump_flag = 0;
	perf_printf("when finish dump, please input set g_dump_flag=1 to continue\n");
	while(!g_dump_flag) {}
	for (int i = 0; i < cpu_num; i++)
		g_perf_data[i].state = RECORD_STATE_BUF_DUMP_END;
	perf_printf("all dump finish.\n");
#endif

#if CONFIG_PERF_DATA_SEMIHOST2_DUMP
	// TODO: cpu0 to dump data
	long fd, rc;
	char filename[32];
	void *databuf;
	long data_len;

	for (int i = 0; i < cpu_num; i++) {
		if (g_perf_data[i].buf_idx == 0)
			continue;
		databuf = g_perf_data[i].buf;
		data_len = g_perf_data[i].buf_idx * sizeof(unsigned long);
		snprintf(filename, sizeof(filename), "perf_rtos_%d.data", i);
		perf_printf("start to dump [%s] perf data[len: %ld] with semihost.[start: %u ms]\n", filename, data_len, csi_tick_get_ms());
		semihost_printf("start to dump [%s] perf data[len: %ld] with semihost.[start: %u ms]\n", filename, data_len, csi_tick_get_ms());

		/* Open in write mode */
		fd = semihost_open(filename, SEMIHOST_OPEN_WB);
		SEMI_CHECK_RET_WITH_GOTO(fd > 0, error);
		rc = semihost_flen(fd);
		if (rc) {
			perf_printf("File may be not empty, rc = %d\r\n", (int)rc);
			goto error;
		}

		/* Write some data */
		rc = semihost_write(fd, databuf, data_len);
		SEMI_CHECK_RET_WITH_GOTO(rc == 0, error);
		rc = semihost_flen(fd);
		SEMI_CHECK_RET_WITH_GOTO(rc == data_len, error);

		/* Close the file */
		rc = semihost_close(fd);
		SEMI_CHECK_RET_WITH_GOTO(rc == 0, error);
		semihost_printf("success dump %s with semihost.[end: %u ms]\n", filename, csi_tick_get_ms());
		perf_printf("success dump %s with semihost.[end: %u ms]\n", filename, csi_tick_get_ms());
	}
	for (int i = 0; i < cpu_num; i++)
		g_perf_data[i].state = RECORD_STATE_BUF_DUMP_END;
	return 0;
error:
	perf_printf("failed dump perf data with semihost.\n");
	for (int i = 0; i < cpu_num; i++)
		g_perf_data[i].state = RECORD_STATE_BUF_DUMP_END;
	return -1;
#endif
	return 0;
}