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

aos_timer_t test_timer;

static void timer_handler(void *arg1, void* arg2)
{
    printf("timer handler\r\n");
}

void test_timer_periodic(void)
{
    int ret = -1;

    printf("test timer periodic\n");

    /*创建定时周期为200ms的周期执行的定时器，并自动运行*/
    ret = aos_timer_new(&test_timer, timer_handler, NULL, 200, 1);
    if (ret != 0) {
        printf("timer create failed\r\n");
    }

    aos_msleep(1000);

    /*停止定时器*/
    aos_timer_stop(&test_timer);

    /*释放定时器*/
    aos_timer_free(&test_timer);
}

void test_timer_change(void)
{
    int ret = -1;

    printf("test timer change time\n");

    /*创建定时周期为200ms的周期执行的定时器，不自动运行*/
    ret = aos_timer_new_ext(&test_timer, timer_handler, NULL, 200, 0, 0);
    if (ret != 0) {
        printf("timer create failed\r\n");
    }

    /*需要手动启动定时器*/
    aos_timer_start(&test_timer);

    aos_msleep(1000);

    /*停止定时器*/
    aos_timer_stop(&test_timer);

    /*改变定时周期为300ms, 注意：需要在定时器未启动状态是才能修改*/
    aos_timer_change(&test_timer, 3000);

    /*启动定时器*/
    aos_timer_start(&test_timer);

    /*停止定时器*/
    aos_timer_stop(&test_timer);

    /*释放定时器*/
    aos_timer_free(&test_timer);
}

static void cmd_timer_func(char *wbuf, int wbuf_len, int argc, char **argv)
{
    if(argc < 2)
        return;

    if(strcmp(argv[1], "auto") == 0){
        test_timer_periodic();
    } else if(strcmp(argv[1], "manual") == 0){
        test_timer_change();
    } else {
        printf("param error\n");
    }
}

void test_aos_timer_cmd(void)
{
    static const struct cli_command cmd_info = {"aos_timer", "aos_timer auto/manual", cmd_timer_func};

    aos_cli_register_command(&cmd_info);
}

