/*
 * Copyright (C) 2023 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>

#include <aos/cli.h>
#include <aos/debug.h>

extern void start_jquick_debugger();
extern void jquick_set_screen_on(int ON);

static void cmd_miniapp_func(char *wbuf, int wbuf_len, int argc, char **argv)
{
    if (argc < 2) {
        return;
    }

    if (strcmp(argv[1], "debug") == 0) {
        start_jquick_debugger();
    } else if (strcmp(argv[1], "touch") == 0) {
        if (argc == 3) {
            jquick_set_screen_on(atoi(argv[2]));
        }
    } else {
        ;
    }
}

void cli_reg_cmd_miniapp(void)
{
    static const struct cli_command cmd_info = { "miniapp", "app extend command.", cmd_miniapp_func };

    aos_cli_register_command(&cmd_info);
}
