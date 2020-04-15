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

#define EVENT_FLAG_3 0x000000F0

aos_event_t test_event;

static void task1_entry(void *arg)
{
    aos_msleep(3000);    // 任务休眠1000ms

    printf("eventtask1 send event\n");

    /*设置事件标志(0x000000F0), 或操作*/

    aos_event_set(&test_event, EVENT_FLAG_3, AOS_EVENT_OR);
}

static void task2_entry(void *arg)
{
    uint32_t actl_flags;

    printf("eventtask2 wait event\n");

    /* 获取事件标志(0x000000F0)，或操作 */
    aos_event_get(&test_event, EVENT_FLAG_3, AOS_EVENT_OR, &actl_flags, 0);

    /*
     * try to get flag EVENT_FLAG_3(0x000000F0) with OR operation should wait here,
     * task2 will set the flags with 0x000000F0, and then task1 will continue
     */
    aos_event_get(&test_event, EVENT_FLAG_3, AOS_EVENT_OR, &actl_flags, AOS_WAIT_FOREVER);

    printf("event test successfully!\n");

    /*删除事件*/
    aos_event_free(&test_event);
}

void test_event_start(void)
{
    int ret = -1;
    uint32_t flags = 0;

    aos_msleep(1000);    // 任务休眠1000ms

    /*当前任务：创建事件*/
    ret = aos_event_new(&test_event, flags);
    if (ret != 0) {
        printf("event create failed\n");
        return;
    }

    /*判断事件是否可用*/
    ret = aos_event_is_valid(&test_event);
    if (ret == 0) {
        printf("event is invalid\n");
    }

    aos_task_new("eventtask1", (void *)task1_entry, NULL, 2*1024);
    aos_task_new("eventtask2", (void *)task2_entry, NULL, 2*1024);
}



static void cmd_event_func(char *wbuf, int wbuf_len, int argc, char **argv)
{
    if(argc < 2)
        return;
    if(strcmp(argv[1], "start") == 0){
        test_event_start();
    }
}

void test_event_cmd(void)
{
    static const struct cli_command cmd_info = {"event", "event command", cmd_event_func};

    aos_cli_register_command(&cmd_info);
}

