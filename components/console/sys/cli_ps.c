/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <string.h>
#include <aos/cli.h>
#include <aos/kernel.h>

#include <yoc/uservice.h>

#define HELP_INFO "show tasks"

void cmd_ps_func(char *wbuf, int wbuf_len, int argc, char **argv)
{
    if (argc == 1) {
        aos_task_show_info();
#if defined(CONFIG_DEBUG) && defined(CONFIG_DEBUG_UTASK)
    } else if (argc == 2 && strcmp(argv[1], "utask") == 0) {
        tasks_debug();
#endif
    }else {
        printf("%s\r\n", HELP_INFO);
    }
}

void cli_reg_cmd_ps(void)
{
    static const struct cli_command cmd_info =
    {
        "ps",
        HELP_INFO,
        cmd_ps_func
    };

    aos_cli_register_command(&cmd_info);
}
