/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */


#include <stdlib.h>
#include <string.h>
#include <aos/aos.h>
#include "aos/cli.h"
#include "main.h"
#include "app_init.h"

#define TAG "app"

void at_channel_init(utask_t *task);
void at_tests_case(void);

void cmd_at_func(char *wbuf, int wbuf_len, int argc, char **argv)
{
    if ((argc == 2) && (0 == strcmp(argv[1], "test"))) {
        at_tests_case();
    } 
}

void cli_reg_cmd_at(void)
{
    static const struct cli_command cmd_info = {
        "at",
        "at",
        cmd_at_func
    };

    aos_cli_register_command(&cmd_info);
}
int main(void)
{
    board_yoc_init();
    LOGD(TAG, "%s\n", aos_get_app_version());

    at_channel_init(NULL);
    cli_reg_cmd_at();

    return 0;
}

