/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <aos/cli.h>

#include "app_disp.h"

/*************************************************
 * 通过命令行控制灯效显示
 *************************************************/
extern char *g_display_show_desc[DISP_SHOW_COUNT];

static void cmd_display_func(char *wbuf, int wbuf_len, int argc, char **argv)
{
    if (argc == 2) {
        int display_id = atoi(argv[1]);
        app_display_show(display_id);
    } else {
        printf("usage:dispshow id\r\n");
        for(int i = 0; i < DISP_SHOW_COUNT; i++) {
            printf("\t%d -> %s\r\n", i, g_display_show_desc[i]);
        }
    }
}

void cli_reg_cmd_display(void)
{
    static const struct cli_command cmd_info = { "dispshow", "display show test", cmd_display_func };

    aos_cli_register_command(&cmd_info);
}
