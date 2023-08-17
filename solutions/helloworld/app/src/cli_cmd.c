/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <aos/kernel.h>
#include <aos/cli.h>
#include "app_main.h"

static void version_cmd(char *buf, int32_t len, int32_t argc, char **argv)
{
    aos_cli_printf("kernel version : %s\r\n", aos_kernel_version_get());
}

static int cli_reg_cmd_kernel(void)
{
	static const struct cli_command cmd_info = {
        "sysver",
        "sysver",
        version_cmd,
	};
	aos_cli_register_command(&cmd_info);
	return 0;
}

void board_cli_init(void)
{
    aos_cli_init();
    cli_reg_cmd_kernel();
}
