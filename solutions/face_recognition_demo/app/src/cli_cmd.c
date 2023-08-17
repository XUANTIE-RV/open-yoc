/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <vfs_cli.h>

extern void cli_reg_cmd_ps(void);
extern void cli_reg_cmd_free(void);
extern void cli_reg_cmd_ping(void);
extern void cli_reg_cmd_kvtool(void);
extern void cli_reg_cmd_iperf(void);
extern int cli_reg_cmd_video_parser(void);
extern int cli_reg_cmd_mpegts(void);
extern void cli_reg_cmd_faceai(void);
extern void cli_reg_cmd_ifconfig(void);

void board_cli_init(void) 
{
    cli_reg_cmd_ps();
    cli_reg_cmd_free();
    cli_reg_cmd_ping();
    cli_reg_cmd_ifconfig();
    cli_reg_cmd_ls();
    cli_reg_cmd_rm();
    cli_reg_cmd_kvtool();
    cli_reg_cmd_iperf();
    cli_reg_cmd_faceai();
}