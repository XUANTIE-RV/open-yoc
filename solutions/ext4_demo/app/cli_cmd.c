/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <aos/cli.h>
#include <vfs_cli.h>

extern void debug_cli_cmd_init(void);
extern void cli_reg_cmd_lext4(void);

void board_cli_init()
{
    aos_cli_init();
    debug_cli_cmd_init();
    cli_reg_cmd_ls();
    cli_reg_cmd_rm();
    cli_reg_cmd_cat();
    cli_reg_cmd_mkdir();
    cli_reg_cmd_mv();
    cli_reg_cmd_lext4();
}
