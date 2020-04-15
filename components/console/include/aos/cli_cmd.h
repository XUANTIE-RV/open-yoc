/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef YOC_CLI_CMD_H
#define YOC_CLI_CMD_H

#ifdef __cplusplus
extern "C" {
#endif

extern void cli_reg_cmd_help(void);
extern void cli_reg_cmd_ping(void);
extern void cli_reg_cmd_ifconfig(void);
extern void cli_reg_cmd_ifconfig_wifi(void);
extern void cli_reg_cmd_ifconfig_eth(void);
extern void cli_reg_cmd_ifconfig_gprs(void);

extern void cli_reg_cmd_ntp(void);
extern void cli_reg_cmd_ps(void);
extern void cli_reg_cmd_free(void);
extern void cli_reg_cmd_factory(void);
extern void cli_reg_cmd_sysinfo(void);
extern void cli_reg_cmd_sysconf(void);
extern void cli_reg_cmd_addr(void);
extern void cli_reg_cmd_kvtool(void);
extern void cli_test_register(void);
extern void cli_reg_cmd_ble();

extern void cli_reg_cmd_ls(void);
extern void cli_reg_cmd_rm(void);
extern void cli_reg_cmd_cat(void);
extern void cli_reg_cmd_mkdir(void);
extern void cli_reg_cmd_mv(void);

extern void cli_reg_cmd_iperf(void);

extern void test_yunit_test_register_cmd();

#ifdef __cplusplus
}
#endif


#endif
