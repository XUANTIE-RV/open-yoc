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
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <rtthread.h>
#include <perf.h>
#include <csi_core.h>

#define CLI_CMD_SIZE 10240
#define PERF_RECORD_CMD_PRE "perf record"
#define PERF_RECORD_CMD_HELP "help"
#define PERF_RECORD_CMD_STOP "perf record stop"

#define PERF_STAT_CMD_PRE "perf stat"
#define PERF_STAT_CMD_HELP "help"
#define PERF_STAT_CMD_STOP "perf stat stop"
#define PERF_LIST_CMD "perf list"

#define PS_CMD "ps"


extern char rt_hw_console_getchar(void);
extern int perf_record_test(int duration, int frequency);
static void simple_cli_task_entry(void *parameter);
extern perf_stat_args_t* perf_stat_parse_cmd(char *input);

struct record_args {
    int duration;
    int frequency;
};

struct cli_shell
{
    char line[CLI_CMD_SIZE + 1];
    int line_position;
};
static struct cli_shell g_cli_shell;

#if defined(CONFIG_SMP) && CONFIG_SMP
static struct record_args g_record_args;
static volatile int g_record_stop;
rt_sem_t g_sem_stat[CONFIG_NR_CPUS];

static void perf_thread_entry(void *parameter)
{
    struct record_args *args = (struct record_args *)parameter;
    perf_record_test(args->duration, args->frequency);
    while(!g_record_stop) {
        rt_thread_mdelay(10);
    }
    perf_record_stop();
}

static void smp_perf_record_test(struct record_args *args)
{
    for (long i = 0; i < CONFIG_NR_CPUS; i++) {
        char buf[32];
        rt_thread_t tid;
        snprintf(buf, sizeof(buf), "record_%ld", i);
        tid = rt_thread_create(buf, perf_thread_entry, args, CONFIG_APP_TASK_STACK_SIZE, 32, 5);
        rt_thread_control(tid, RT_THREAD_CTRL_BIND_CPU, (void *)i);
        if (tid != RT_NULL) {
            rt_thread_startup(tid);
        }
    }
}

static void perf_stat_thread_entry(void *parameter)
{
    perf_stat_args_t *args = (perf_stat_args_t *) parameter;
    int cpu_id = csi_get_cpu_id();
    // start perf_stat
    perf_stat_start(args);
    // wait
    if (rt_sem_take(g_sem_stat[cpu_id], RT_WAITING_FOREVER) != RT_EOK)
    {
        perf_printf("rt_sem_take() failed\r\n");
    }
    perf_stat_stop();
}

static void smp_perf_stat_test(perf_stat_args_t *args)
{
    for (long i = 0; i < CONFIG_NR_CPUS; i++)
    {
        char buf[32];
        rt_thread_t tid;
        snprintf(buf, sizeof(buf), "stat_%ld", i);
        tid = rt_thread_create(buf, perf_stat_thread_entry, args, CONFIG_APP_TASK_STACK_SIZE, 32, 5);
        rt_thread_control(tid, RT_THREAD_CTRL_BIND_CPU, (void *)i);
        if (tid != RT_NULL)
        {
            rt_thread_startup(tid);
        }
    }
}
#endif

static void simple_cli_task_entry(void *parameter)
{
    int ch;
    int numbers[2];
    int numberCount = 0;

    memset(&g_cli_shell, 0, sizeof(g_cli_shell));

    while (1)
    {
        ch = rt_hw_console_getchar();
        if (ch < 0) {
            continue;
        }
        if (g_cli_shell.line_position > CLI_CMD_SIZE)
            g_cli_shell.line_position = 0;
        g_cli_shell.line[g_cli_shell.line_position++] = ch;
        rt_kprintf("%c", ch);
        if (ch == 0x0d) {
            rt_kprintf("\n");
            char *subs = strstr((char *)&g_cli_shell.line[0], PERF_RECORD_CMD_PRE);
            char *subs_perf_stat = strstr((char *)&g_cli_shell.line[0], PERF_STAT_CMD_PRE);
            char *subs_list_threads = strstr((char *)&g_cli_shell.line[0], PS_CMD);
            char *sub_list_events = strstr((char *)&g_cli_shell.line[0], PERF_LIST_CMD);
            int pos = 0;
            if (subs) {
                if (!strncmp(subs, PERF_RECORD_CMD_STOP, strlen(PERF_RECORD_CMD_STOP))) {
#if defined(CONFIG_SMP) && CONFIG_SMP
                    g_record_stop = true;
#else
                    perf_record_stop();
#endif
                } else {
                    char numc[16];
                    numberCount = 0;
                    char *ptr = subs + strlen(PERF_RECORD_CMD_PRE);
                    while(numberCount < 2) {
                        for (int i = 0; i < g_cli_shell.line_position - strlen(PERF_RECORD_CMD_PRE) - pos; i++) {
                            if (*ptr && *ptr != 0x20) {
                                numc[pos++] = *ptr;
                            }
                            if (pos > 0 && *ptr == 0x20) {
                                break;
                            }
                            ptr++;
                        }
                        numc[pos] = 0;
                        pos = 0;
                        char *ptr1;
                        numbers[numberCount] = strtol(numc, &ptr1, 10);
                        numberCount++;
                    }
                    if (numbers[0] > 0 && numbers[1] > 0) {
#if defined(CONFIG_SMP) && CONFIG_SMP
                        g_record_stop = false;
                        g_record_args.duration = numbers[0];
                        g_record_args.frequency = numbers[1];
                        smp_perf_record_test(&g_record_args);
#else
                        perf_record_test(numbers[0], numbers[1]);
#endif
                    }
                }
            } else if (subs_perf_stat) {
                if (!strncmp(subs_perf_stat, PERF_STAT_CMD_STOP, strlen(PERF_STAT_CMD_STOP))) {
                    rt_kprintf("perf_stat_stop()\r\n");
#if defined(CONFIG_SMP) && CONFIG_SMP
                    for (int i = 0; i < CONFIG_NR_CPUS; i++) {
                        if(rt_sem_release(g_sem_stat[CONFIG_NR_CPUS - 1 - i]) != RT_EOK) {
                            rt_kprintf("cli: rt_sem_release() failed\r\n");
                        }
                    }
#else
                    perf_stat_stop();
#endif
                }
                else {
                    rt_kprintf("perf_stat_start()\r\n");
                    perf_stat_args_t* stat_args = perf_stat_parse_cmd(subs_perf_stat);
#if defined(CONFIG_SMP) && CONFIG_SMP
                    smp_perf_stat_test(stat_args);
#else
                    perf_stat_start(stat_args);
#endif
                }
            } else if (subs_list_threads) {
                if(!strncmp(subs_list_threads, PS_CMD, strlen(PS_CMD)))
                rt_kprintf("current threads:\r\n");
                extern long list_thread(void);
                list_thread();
            } else if (sub_list_events) {
                if(!strncmp(sub_list_events, PERF_LIST_CMD, strlen(PERF_LIST_CMD))) {
                    perf_list_events();
                }
            } else if (!strncmp(g_cli_shell.line, "help", strlen("help"))) {
                rt_kprintf("perf record <duration_ms> <frequency>\r\n"
                           "perf stat -e ev1,ev2,ev3 -C 0,1 -t 0x1e23,0x3f52,0x12345678\r\n");
            }
            g_cli_shell.line_position = 0;
            memset(&g_cli_shell.line[0], 0, sizeof(g_cli_shell.line));
        }
    }
}

int simple_cli_task_create(void)
{
    rt_thread_t tid;
#if defined(CONFIG_SMP) && CONFIG_SMP
    for (int i = 0; i < CONFIG_NR_CPUS; i++) {
        g_sem_stat[i] = rt_sem_create("g_sem_stat", 0, RT_IPC_FLAG_PRIO);
        if (g_sem_stat[i] == RT_NULL) {
            rt_kprintf("rt_sem_create() failed\r\n");
        }
    }
#endif
    tid = rt_thread_create("uart_rx", simple_cli_task_entry, RT_NULL, CONFIG_APP_TASK_STACK_SIZE, 60, 10);
    rt_thread_control(tid, RT_THREAD_CTRL_BIND_CPU, (void *)0);
    if (tid != RT_NULL) {
        rt_thread_startup(tid);
        return 0;
    }
    return -RT_ERROR;
}
