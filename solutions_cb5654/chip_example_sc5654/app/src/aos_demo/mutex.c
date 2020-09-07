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

aos_mutex_t test_mutex;

void task1_entry(void)
{
    int ret = -1;
    int i = 0;
    while (i < 10) {
        ret = aos_mutex_is_valid(&test_mutex);
        if (ret != 1) {
            return;
        }
        ret = aos_mutex_lock(&test_mutex, AOS_WAIT_FOREVER);
        if (ret != 0) {
            continue;
        }
        /*访问临界资源*/
        printf("task1 entry access critical zone\n");

        aos_mutex_unlock(&test_mutex);
		i++;
    }
	
	/* 任务退出 */
    aos_task_exit(0);
}

void task2_entry(void)
{
    int ret = -1;
	int i = 0;
    while (i < 10) {
        ret = aos_mutex_is_valid(&test_mutex);
        if (ret != 1) {
            return;
        }
        ret = aos_mutex_lock(&test_mutex, 1000);
        if (ret != 0) {
            continue;
        }
        /*访问临界资源*/
        printf("task2 entry access critical zone\n");

        aos_mutex_unlock(&test_mutex);
		i++;
    }
	
	/* 任务退出 */
    aos_task_exit(0);
}

static void cmd_mutex_func(char *wbuf, int wbuf_len, int argc, char **argv)
{
    if(argc < 2)
        return;

    if(strcmp(argv[1], "start") == 0){
        int ret = -1;
        ret = aos_mutex_new(&test_mutex);
        if (ret != 0) {
            return;
        }
        aos_task_new("mutextask1", (void *)task1_entry, NULL, 2*1024);
        aos_task_new("mutextask2", (void *)task2_entry, NULL, 2*1024);
    } else {
        printf("param error\n");
    }
}

void test_mutex_cmd(void)
{
    static const struct cli_command cmd_info = {"mutex", "mutex command", cmd_mutex_func};

    aos_cli_register_command(&cmd_info);
}

