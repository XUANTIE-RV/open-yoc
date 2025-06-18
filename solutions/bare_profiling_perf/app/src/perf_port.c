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
#include "app_init.h"

cpu_bind_timer_t g_cpu_bind_timer[8] = {
	{0, 0},
	{1, 1},
	{2, 2},
	{3, 3}
};

spinlock_t g_print_spinlock;
spinlock_t g_alloc_bufdata_spinlock;

void perf_spinlock_init(void)
{
	spin_lock_init(&g_print_spinlock);
	spin_lock_init(&g_alloc_bufdata_spinlock);
}

int perf_printf(const char *fmt, ...)
{
    int ret;
    va_list args;

	unsigned long level = csi_irq_save();
	spin_lock(&g_print_spinlock);
	printf("cpuid[%d] ", csi_get_cpu_id());
    va_start(args, fmt);
    ret = vprintf(fmt, args);
    va_end(args);
	spin_unlock(&g_print_spinlock);
	csi_irq_restore(level);

    return ret;
}

void *perf_malloc(size_t size)
{
    spin_lock(&g_alloc_bufdata_spinlock);
    unsigned long *p = (unsigned long *)malloc(size);
    if (p == NULL) {
        perf_printf("perf data buf malloc failed. cpu:%d\n", csi_get_cpu_id());
        spin_unlock(&g_alloc_bufdata_spinlock);
        return NULL;
    }
    spin_unlock(&g_alloc_bufdata_spinlock);
    return p;
}

void *perf_realloc(void *ptr, size_t size)
{
    spin_lock(&g_alloc_bufdata_spinlock);
    unsigned long *p = (unsigned long *)realloc(ptr, size);
    if (p == NULL) {
        perf_printf("perf data buf malloc failed. cpu:%d\n", csi_get_cpu_id());
        spin_unlock(&g_alloc_bufdata_spinlock);
        return NULL;
    }
    spin_unlock(&g_alloc_bufdata_spinlock);
    return p;
}

void perf_free(void* ptr)
{
    if (ptr) {
        spin_lock(&g_alloc_bufdata_spinlock);
        free(ptr);
        spin_unlock(&g_alloc_bufdata_spinlock);
    }
}

size_t perf_record_per_core_buffer_size(void)
{
    return CONFIG_PROFILING_PERF_PER_CORE_BUFFER_SIZE;
}

unsigned long *perf_record_alloc_data_buf(void)
{
	spin_lock(&g_alloc_bufdata_spinlock);
	unsigned long *p = (unsigned long *)malloc(perf_record_per_core_buffer_size());
	if (p == NULL) {
		perf_printf("perf data buf malloc failed. cpu:%d\n", csi_get_cpu_id());
		spin_unlock(&g_alloc_bufdata_spinlock);
		return NULL;
	}
	spin_unlock(&g_alloc_bufdata_spinlock);
	return p;
}

void perf_record_free_data_buf(unsigned long *buf)
{
	if (buf) {
		spin_lock(&g_alloc_bufdata_spinlock);
		perf_printf("will free databuf[0x%lx]\n", (unsigned long)buf);
		free(buf);
		perf_printf("free databuf[0x%lx] ok\n", (unsigned long)buf);
		spin_unlock(&g_alloc_bufdata_spinlock);
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