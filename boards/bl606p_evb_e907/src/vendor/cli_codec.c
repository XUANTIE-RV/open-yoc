/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <aos/cli.h>

extern void cmd_micgain(char *wbuf, int wbuf_len, int argc, char **argv);

void cli_reg_cmd_codec(void)
{
    static const struct cli_command cmd_list[] = {
        {"micgain", "show mic config", cmd_micgain},
    };

    aos_cli_register_commands(cmd_list, sizeof(cmd_list)/sizeof(struct cli_command));
}

