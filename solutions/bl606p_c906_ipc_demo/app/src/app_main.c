/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <string.h>
#include <aos/kernel.h>
#include <ulog/ulog.h>
#include <ipc.h>
#include "app_main.h"

#define TAG "C906"

extern void cxx_system_init(void);

ipc_t *ipc_send = NULL;
ipc_t *ipc_recv = NULL;
const char transfer_data[] = "hello world";
const char cli_data[] = "cli";

static int ipc_sync(void)
{
    message_t msg;

    msg.command = 103;
    msg.flag    = MESSAGE_SYNC;
    msg.service_id = 20;
    msg.resp_data = NULL;
    msg.resp_len = 0;

    int snd_len = 4096;
    int offset = 0;
    char *send = (char *)transfer_data;


    while (offset < sizeof(transfer_data)) {
        msg.req_data    = send + offset;

        snd_len = 4096 < (sizeof(transfer_data) - offset)? 4096 : (sizeof(transfer_data) - offset);
        msg.req_len     = snd_len;

        ipc_message_send(ipc_send, &msg, AOS_WAIT_FOREVER);

        offset += snd_len;
    }

    LOGI(TAG, "ipc async done\n");
    return 0;
}

static int ipc_client_init(void)
{
    /* get ipc （cpu_id:1）*/
    ipc_send = ipc_get(1);

    /* add ipc service（service_id：20）*/
    ipc_add_service(ipc_send, 20, NULL, NULL);

    ipc_sync();

    return 0;
}

static void task_ipc_cli(void *priv)
{
    LOGI(TAG, "ipc cli task, cli_data = %s\n", cli_data);
}

static void cli_ipc_process(ipc_t *ipc, message_t *msg, void *priv)
{
    if (msg->command == 103) {

        if (0 == memcmp(cli_data, msg->req_data, strlen(msg->req_data))) {
            aos_task_t task;
            aos_task_new_ext(&task, "ipc_cli", task_ipc_cli, NULL, 4096, 32); // internuclear cli
        } else {
            LOGI(TAG, "music recv ok, msg->req_data = %s\n", msg->req_data);
        }

        ipc_message_ack(ipc, msg, AOS_WAIT_FOREVER);
    }
}

static int ipc_service_init(void)
{
    /* get ipc （cpu_id:0）*/
    ipc_recv = ipc_get(1);

    /* add ipc service（service_id：20）*/
    ipc_add_service(ipc_recv, 10, cli_ipc_process, NULL);

    return 0;
}

int main(int argc, char *argv[])
{
    cxx_system_init();
    board_yoc_init();

    LOGI(TAG, "ipc client test start\n");

    ipc_client_init();

    LOGI(TAG, "ipc service test start\n");

    ipc_service_init();
}