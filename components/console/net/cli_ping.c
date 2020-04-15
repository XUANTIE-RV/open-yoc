/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <yoc_config.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#include <aos/kernel.h>
#include <aos/cli.h>

extern int ping(int type, char *remote_ip);

static void cmd_ping_func(char *wbuf, int wbuf_len, int argc, char **argv)
{
    int ping_type = 0;
    char *host_ip;

    if (argc < 2) {
        printf("Usage: ping dest or ping -6 dest\n");
        return;
    }

    if (strcmp(argv[1], "-6") == 0) {
        ping_type = 6;
        host_ip = argv[2];
    } else {
        ping_type = 4;
        host_ip = argv[1];
    }

    ping(ping_type, host_ip);
}

void cli_reg_cmd_ping(void)
{
    static const struct cli_command cmd_info = {
        "ping",
        "ping command.",
        cmd_ping_func
    };

    aos_cli_register_command(&cmd_info);
}

