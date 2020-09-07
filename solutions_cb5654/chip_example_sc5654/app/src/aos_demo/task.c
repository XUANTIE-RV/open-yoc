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

static void test_task(void *paras)
{
    int ret = -1;
    int var = 0;
    aos_task_key_t task_key;
    void *task_value = NULL;

     /* 创建任务私有数据区域*/
    ret = aos_task_key_create(&task_key);
    if (ret) {
        printf("task key create failed\n");
    }

    /* 打印任务名和任务私有数据区域索引值*/
    printf("%s task key 0x%x. \r\n", aos_task_name(), task_key);

    aos_msleep(10);    // 任务休眠10ms
    var = 0x5a5a;
     /* 设置当前任务私有数据区域的某索引空闲块内容 */
    ret = aos_task_setspecific(task_key, &var);

    /* 获取当前任务私有数据区域的某索引数据块内容 */
    task_value = aos_task_getspecific(task_key);
    printf("%s task key 0x%x. \r\n", aos_task_name(), *(int*)task_value);

    /* 删除任务私有数据区域的空闲块索引 */
    aos_task_key_delete(task_key);

    /* 任务退出 */
    aos_task_exit(0);
}

static void cmd_task_func(char *wbuf, int wbuf_len, int argc, char **argv)
{
    if(argc < 2)
        return;

    if(strcmp(argv[1], "start") == 0){
        /* 创建任务test_task，任务栈大小为1024字节 */
        aos_task_new("test_task", (void *)test_task, NULL, 2*1024);
    } else {
        printf("param error\n");
    }
}

void test_task_cmd(void)
{
    static const struct cli_command cmd_info = {"task", "task command", cmd_task_func};

    aos_cli_register_command(&cmd_info);
}

