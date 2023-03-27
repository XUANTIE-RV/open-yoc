/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <aos/cli.h>

extern void hal_rtc_set_fel_flag();
static void efex_cmd(char *buf, int len, int argc, char **argv)
{
    hal_rtc_set_fel_flag();
}

static const struct cli_command cmd_xfex_specific = {
    /*cli self*/
    "xfex", "Enter xfex mode to burn image", efex_cmd
};

void cli_reg_cmd_board_specific(void)
{
    aos_cli_register_command(&cmd_xfex_specific);
}
