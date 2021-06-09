/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#include <aos/kernel.h>
#include <aos/cli.h>

static void cmd_debug_func(char *wbuf, int wbuf_len, int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: debugfunc CMD\n");
        return;
    }

    if (strcmp(argv[1], "lvgl") == 0) {
        if (argc < 3) {
            printf("Input mesage to show\n");
        } else {
            extern void show_mbox_msg(const char * txt, const int tm_ms);
            show_mbox_msg(argv[2], 2000);
        }
    } else {
        printf("Wrong cmd parameter.\n");
    }
}

void cli_reg_cmd_debug_func(void)
{
    static const struct cli_command cmd_info = {
        "tstfun",
        "testfunc command for debug function.",
        cmd_debug_func
    };

    aos_cli_register_command(&cmd_info);
}
