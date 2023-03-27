/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <string.h>
#include <aos/kernel.h>
#include <aos/cli.h>
#include <aos/yloop.h>
#include <ulog/ulog.h>
#include <ipc.h>
#include "app_main.h"

#define TAG "AP"

extern void cxx_system_init(void);

ipc_t *ipc_recv = NULL;
ipc_t *ipc_send = NULL;
const char transfer_data[] = "hello world";
const char cli_data[] = "cli";

static void cli_ipc_process(ipc_t *ipc, message_t *msg, void *priv)
{
    if (msg->command == 103) {
        char *music = (char *)transfer_data;
        static int music_len = 0;

        int ret = memcmp(music + music_len, msg->req_data, msg->req_len);
        music_len += msg->req_len;

        if (ret != 0) {
            LOGI(TAG, "ipc sync err!!!\n");
        }

        if (music_len == sizeof(transfer_data)) {
            LOGI(TAG, "music recv ok, total:(%d)\n", music_len);
            LOGI(TAG, "music recv ok, music = %s\n", music);
            music_len = 0;
        }

        ipc_message_ack(ipc, msg, AOS_WAIT_FOREVER);

    }
}

static int ipc_service_init(void)
{
    /* get ipc （cpu_id:0）*/
    ipc_recv = ipc_get(0);

    /* add ipc service（service_id：20）*/
    ipc_add_service(ipc_recv, 20, cli_ipc_process, NULL);

    return 0;
}

static int ipc_sync(void)
{
    message_t msg;

    msg.command = 103;
    msg.flag    = MESSAGE_SYNC;
    msg.service_id = 10;
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
    /* get ipc （cpu_id:0）*/
    ipc_send = ipc_get(0);

    /* add ipc service（service_id：20）*/
    ipc_add_service(ipc_send, 10, NULL, NULL);

    ipc_sync();

    return 0;
}

// internuclear cli
static int ipc_cli_sync(void)
{
    message_t msg;

    msg.command = 103;
    msg.flag    = MESSAGE_SYNC;
    msg.service_id = 10;
    msg.resp_data = NULL;
    msg.resp_len = 0;

    int snd_len = 4096;
    int offset = 0;
    char *send = (char *)cli_data;

    while (offset < sizeof(cli_data)) {
        msg.req_data    = send + offset;

        snd_len = 4096 < (sizeof(cli_data) - offset)? 4096 : (sizeof(cli_data) - offset);
        msg.req_len     = snd_len;

        ipc_message_send(ipc_send, &msg, AOS_WAIT_FOREVER);

        offset += snd_len;
    }

    LOGI(TAG, "ipc async done\n");
    return 0;
}

const char* help_str = "ipc cli demo test\n"
    "ipcdemo ipc_log\n"
    "for more information see the README\n\n>";


static void appdemohal_cmd(char *wbuf, int wbuf_len, int argc, char **argv)
{
    if (argc < 2) {
        goto help;
    }

    if (strcmp(argv[1], "ipc_log") == 0) {
        ipc_cli_sync();
    }

    return;

help:
    printf("Argument failed\n%s", help_str);
}

static void ipc_cli_init(void)
{
    static const struct cli_command cmd_info = {
        "ipcdemo",
        "ipcdemo test",
        appdemohal_cmd
    };
    aos_cli_register_command(&cmd_info);
}

int main(int argc, char *argv[])
{
    
    cxx_system_init();
    board_yoc_init();

    LOGI(TAG, "ipc service test start\n");

    ipc_service_init();

    LOGI(TAG, "aos_msleep 1s, wait for the c906 to receive the ack to complete");
    aos_msleep(1000);

    LOGI(TAG, "ipc client test start\n");

    ipc_client_init();

    LOGI(TAG, "internuclear test start\n");

    ipc_cli_init();
}