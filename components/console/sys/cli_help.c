/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <yoc_config.h>

#include <aos/cli.h>
#include <aos/cli.h>

static uint32_t help_func(void)
{
    slist_t cmd_lists = cli_service_get_cmd_list();

    cmd_list_t *node;

    slist_for_each_entry(&cmd_lists, node, cmd_list_t, next) {
        printf("%-15s : %s\n", node->cmd->name, node->cmd->help);
    }

    return 0;
}

static void cmd_help_func(char *wbuf, int wbuf_len, int argc, char **argv)
{
    if (NULL == argv[1]) {
        help_func();
    }
}

void cli_reg_cmd_help(void)
{
    static const struct cli_command cmd_info = {
        "help",
        "show commands",
        cmd_help_func,
    };

    aos_cli_register_command(&cmd_info);
}
