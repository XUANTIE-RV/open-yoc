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

#define QUEUE_BUF_LEN 100

static aos_queue_t test_queue;
static char queue_buf[QUEUE_BUF_LEN];

static void task1_entry(void *arg)
{
    char *msg_send = "Hello, Queue!";

    aos_msleep(3000);    // 任务休眠1000ms

    printf("queuetask1 send msg\n");

    /*发送消息*/
    aos_queue_send(&test_queue, msg_send, strlen(msg_send));
}

static void task2_entry(void *arg)
{
    char msg_recv[16] = {0};
    unsigned int size_recv = 16;

    printf("queuetask2 wait msg\n");

    memset(msg_recv, 0, size_recv);
    aos_queue_recv(&test_queue, 100000, msg_recv, &size_recv);

    printf("queuetask2 get msg: ");
    for (int i = 0; i < size_recv; i++) {
        printf("%c", msg_recv[i]);
    }
    printf("\n");

    /*获取到消息，当前任务继续执行下去*/
    printf("queue test successfully!\n");

    /*删除消息队列*/
    aos_queue_free(&test_queue);
}

static void cmd_queue_func(char *wbuf, int wbuf_len, int argc, char **argv)
{
    if(argc < 2)
        return;

    if(strcmp(argv[1], "start") == 0){
        int ret = -1;
        /*当前任务：创建消息队列，消息队列最大为长度为100，单条消息最大为20*/
        ret = aos_queue_new(&test_queue, queue_buf, QUEUE_BUF_LEN, 20);
        if (ret != 0) {
            printf("queue create failed\n");
            return;
        }
        
        aos_task_new("queuetask1", (void *)task1_entry, NULL, 1024);
        aos_task_new("queuetask2", (void *)task2_entry, NULL, 1024);
    } else {
        printf("param error\n");
    }
}

void test_queue_cmd(void)
{
    static const struct cli_command cmd_info = {"queue", "queue command", cmd_queue_func};

    aos_cli_register_command(&cmd_info);
}

