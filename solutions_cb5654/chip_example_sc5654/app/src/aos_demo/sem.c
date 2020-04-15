/*
 * Copyright (C) 2017-2019 C-SKY Microsystems Co., All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <app_config.h>

#include <stdlib.h>
#include <string.h>

#include <aos/aos.h>
#include <aos/cli.h>

#include <yoc/sysinfo.h>
#include <yoc/uservice.h>
#include <yoc/eventid.h>
#include <devices/devicelist.h>

#include "hw_config.h"

#define TAG "app"

aos_sem_t test_sem;

static void task1_entry(void *arg)
{
    aos_msleep(3000);    // 任务休眠1000ms

    printf("semtask1 send semaphore\n");

    /*释放信号量*/
    aos_sem_signal(&test_sem);
}

static void task2_entry(void *arg)
{
    printf("semtask2 wait semaphore\n");

    /*获取信号量，由于初始值为0，这里获取不到信号量，当前任务进入睡眠并发生切换
      参数 -1 表示AOS_WAIT_FOREVER，直到task1任务释放信号量*/
    aos_sem_wait(&test_sem, -1);

    /*获取到信号量，当前任务继续执行下去*/
    printf("sem test successfully!\n");

    /*删除信号量*/
    aos_sem_free(&test_sem);
}

void test_sem_start(void)
{
    int ret = -1;

    aos_msleep(1000);    // 任务休眠1000ms

    /*当前任务：创建信号量，信号量初始count为0*/
    ret = aos_sem_new(&test_sem, 0);
    if (ret != 0) {
        printf("sem create failed\n");
        return;
    }

    /*判断信号量是否可用*/
    ret = aos_sem_is_valid(&test_sem);
    if (ret == 0) {
        printf("sem is invalid\n");
    }

    aos_task_new("semtask1", (void *)task1_entry, NULL, 1024);
    aos_task_new("semtask2", (void *)task2_entry, NULL, 1024);
}

static void cmd_sem_func(char *wbuf, int wbuf_len, int argc, char **argv)
{
    if(argc < 2)
        return;

    if(strcmp(argv[1], "start") == 0){
        int ret = -1;
        /*当前任务：创建信号量，信号量初始count为0*/
        ret = aos_sem_new(&test_sem, 0);
        if (ret != 0) {
            printf("sem create failed\n");
            return;
        }
        
        /*判断信号量是否可用*/
        ret = aos_sem_is_valid(&test_sem);
        if (ret == 0) {
            printf("sem is invalid\n");
        }
        
        aos_task_new("semtask1", (void *)task1_entry, NULL, 1024);
        aos_task_new("semtask2", (void *)task2_entry, NULL, 1024);
    } else {
        printf("param error\n");
    }
}

void test_sem_cmd(void)
{
    static const struct cli_command cmd_info = {"sem", "sem command", cmd_sem_func};

    aos_cli_register_command(&cmd_info);
}

