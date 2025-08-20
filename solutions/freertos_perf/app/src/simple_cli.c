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
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <perf.h>
#include <csi_core.h>

#define uxTaskInfoBufferLenth (4096)

#define CLI_CMD_SIZE (10240)
#define PERF_RECORD_CMD_PRE "perf record"
#define PERF_RECORD_CMD_HELP "help"
#define PERF_RECORD_CMD_STOP "perf record stop"

#define PERF_STAT_CMD_PRE "perf stat"
#define PERF_STAT_CMD_HELP "help"
#define PERF_STAT_CMD_STOP "perf stat stop"

#define PERF_LIST_CMD "perf list"

#define PS_CMD "ps"

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

static void print_a_task(perf_thread_hashnode_t task)
{
    char* name;
    char state_str[16];
    name = pcTaskGetTaskName((TaskHandle_t)(task->address));
    UBaseType_t state = eTaskGetState(task->address);
    switch (state)
    {
        case eRunning: sprintf(state_str, "running"); break;
        case eReady: sprintf(state_str, "ready"); break;
        case eBlocked: sprintf(state_str, "blocked"); break;
        case eDeleted: sprintf(state_str, "deleted"); break;
    }
    printf("%-10p %-16s %-6d %-12p %-12p %-12p %-8s\n",
        task->address,
        name,
        (int)uxTaskPriorityGet((TaskHandle_t)task->address),
        (unsigned long*)task->tcb->stack_addr_low,
        (unsigned long*)task->tcb->stack_addr_high,
        (unsigned long*)(*(unsigned long*)(task->address)),
        state_str
    );
}

void task_show_info(void)
{
    taskENTER_CRITICAL();
    printf("%-10s %-16s %-6s %-12s %-12s %-12s %-8s\n",
        "tcb addr", "thread", "pri", "stack_low", "stack_high", "sp", "state");
    printf("%-10s %-16s %-6s %-12s %-12s %-12s %-8s\n",
        "-----", "-----", "-----", "-----", "-----", "-----", "-----");
    perf_task_table_traverse(print_a_task);
    taskEXIT_CRITICAL();
}

#if defined(CONFIG_SMP) && CONFIG_SMP
static struct record_args g_record_args;
static volatile int g_record_stop;
SemaphoreHandle_t xSemStat[configNUMBER_OF_CORES];

static void perf_record_thread_entry(void *parameter)
{
    struct record_args *args = (struct record_args *)parameter;
    perf_record_test(args->duration, args->frequency);
    while(!g_record_stop) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    perf_record_stop();
    vTaskDelete(NULL);
}

static void smp_perf_record_test(struct record_args *args)
{
    for (long i = 0; i < configNUMBER_OF_CORES; i++) {
        char buf[32];
        TaskHandle_t xHandle = NULL;
        snprintf(buf, sizeof(buf), "record_%ld", i);
        UBaseType_t uxCoreAffinityMask = (1 << i);
        BaseType_t ret = xTaskCreateAffinitySet(perf_record_thread_entry, buf, CONFIG_APP_TASK_STACK_SIZE / sizeof(StackType_t), args, 32, uxCoreAffinityMask, &xHandle);
        configASSERT(ret == pdPASS);
    }
}

static void perf_stat_thread_entry(void *parameter)
{
    perf_stat_args_t *args = (perf_stat_args_t *) parameter;
    int cpu_id = csi_get_cpu_id();
    // start perf_stat
    perf_stat_start(args);
    // wait
    configASSERT(xSemaphoreTake(xSemStat[cpu_id], portMAX_DELAY) == pdTRUE);
    perf_stat_stop();
    vTaskDelete(NULL);
}

static void smp_perf_stat_test(perf_stat_args_t *args)
{
    for (long i = 0; i < configNUMBER_OF_CORES; i++)
    {
        UBaseType_t uxCoreAffinityMask = (1 << i); // core id
        char buf[32];
        TaskHandle_t xHandle = NULL;
        snprintf(buf, sizeof(buf), "stat_%ld", i);
        BaseType_t ret = xTaskCreateAffinitySet(perf_stat_thread_entry, buf, CONFIG_APP_TASK_STACK_SIZE / sizeof(StackType_t), args, 32, uxCoreAffinityMask, &xHandle);
        configASSERT(ret == pdPASS);
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
        ch = fgetc(NULL);
        if (ch < 0) {
            continue;
        }
        if (g_cli_shell.line_position > CLI_CMD_SIZE)
            g_cli_shell.line_position = 0;
        g_cli_shell.line[g_cli_shell.line_position++] = ch;
        printk("%c", ch);
        fflush(0);
        if (ch == 0x0d) {
            printk("\n");
            char *subs = strstr((char *)&g_cli_shell.line[0], PERF_RECORD_CMD_PRE);
            char *subs_perf_stat = strstr((char *)&g_cli_shell.line[0], PERF_STAT_CMD_PRE);
            char *subs_list_threads = strstr((char *)&g_cli_shell.line[0], PS_CMD);
            char * sub_list_events = strstr((char *)&g_cli_shell.line[0], PERF_LIST_CMD);
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
                    printk("perf_stat_stop()\r\n");
#if defined(CONFIG_SMP) && CONFIG_SMP
                    for (int i = 0; i < configNUMBER_OF_CORES; i++) {
                        configASSERT(xSemaphoreGive(xSemStat[configNUMBER_OF_CORES - 1 - i]) == pdPASS);
                    }
#else
                    perf_stat_stop();
#endif
                }
                else {
                    printk("perf_stat_start()\r\n");
                    perf_stat_args_t* stat_args = perf_stat_parse_cmd(subs_perf_stat);
#if defined(CONFIG_SMP) && CONFIG_SMP
                    smp_perf_stat_test(stat_args);
#else
                    perf_stat_start(stat_args);
#endif
                }
            } else if (subs_list_threads) {
                if(!strncmp(subs_list_threads, PS_CMD, strlen(PS_CMD)))
                task_show_info();
            } else if (sub_list_events) {
                if(!strncmp(sub_list_events, PERF_LIST_CMD, strlen(PERF_LIST_CMD)))
                {
                    perf_list_events();
                }
            } else if (!strncmp(g_cli_shell.line, "help", strlen("help"))) {
                printk("perf record <duration_ms> <frequency>\r\n"
                           "perf stat -e ev1,ev2,ev3 -C 0,1 -t 0x1e23,0x3f52,0x12345678\r\n");
            }
            g_cli_shell.line_position = 0;
            memset(&g_cli_shell.line[0], 0, sizeof(g_cli_shell.line));
        }
    }
}

int simple_cli_task_create(void)
{
    TaskHandle_t xHandle = NULL;
    char buf[32];
    sprintf(buf, "%s", "cli_task");
    extern void perf_spinlock_init(void);
    perf_spinlock_init();
#if defined(CONFIG_SMP) && CONFIG_SMP
    for (int i = 0; i < configNUMBER_OF_CORES; i++) {
        xSemStat[i] = xSemaphoreCreateBinary();
        configASSERT(xSemStat[i] != NULL);
    }
    UBaseType_t uxCoreAffinityMask = (1 << 0);
    BaseType_t ret = xTaskCreateAffinitySet(simple_cli_task_entry, buf, CONFIG_APP_TASK_STACK_SIZE / sizeof(StackType_t), NULL, 32, uxCoreAffinityMask, &xHandle);
#else
    BaseType_t ret = xTaskCreate(simple_cli_task_entry, buf, CONFIG_APP_TASK_STACK_SIZE / sizeof(StackType_t), NULL, 32, &xHandle);
#endif
    configASSERT(ret == pdPASS);
    return pdPASS;
}
