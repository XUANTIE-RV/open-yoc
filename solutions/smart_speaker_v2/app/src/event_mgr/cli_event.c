/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <aos/cli.h>

#include "app_event.h"

extern status_event_t g_status_event_list[];
extern int g_status_event_list_count;

/*************************************************
 * 通过命令行模拟事件发送，用于测试执行体
 *************************************************/
static void cmd_status_event_func(char *wbuf, int wbuf_len, int argc, char **argv)
{
    if (argc == 2) {
        char *endptr;
        char *strid = argv[1];
        int eventid = strtol(&strid[2], &endptr, 16);

        printf("public event id=0x%x\r\n", eventid);
        app_event_update(eventid);
    } else {
        printf("usage:stevt eventid\r\n");
        printf("\teventid list\r\n");
        for (int i = 0; i < g_status_event_list_count; i++) {
            printf("\t0x%0x -> %s\r\n", g_status_event_list[i].eventid, g_status_event_list[i].name);
        }
    }
}

void cli_reg_cmd_status_event(void)
{
    static const struct cli_command cmd_info = { "stevt", "status event test", cmd_status_event_func };

    aos_cli_register_command(&cmd_info);
}
