/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */


#include <aos/cli.h>
#include <aos/cli_cmd.h>
#include <devices/device.h>
#include <devices/led.h>
#include <devices/gprs.h>
#include <devices/gnss.h>
#include <yoc/lpm.h>

#define TAG "CLI APP"

static void cmd_sleep_func(char *wbuf, int wbuf_len, int argc, char **argv)
{
    pm_agree_halt(0);
}

void cli_reg_cmd_sleep(void)
{
    static const struct cli_command cmd_info =
    {
        "sleep",
        "sleep",
        cmd_sleep_func
    };

    aos_cli_register_command(&cmd_info);
}

extern int m6313_passthrough(char *atcmd);
static void cmd_m6313_func(char *wbuf, int wbuf_len, int argc, char **argv)
{
    if (argc == 2) {
        m6313_passthrough(argv[1]);
    }
}

void cli_reg_cmd_m6131_passthrough(void)
{
    static const struct cli_command cmd_info =
    {
        "m6313",
        "m6313 at passthrough",
        cmd_m6313_func
    };

    aos_cli_register_command(&cmd_info);
}

extern void test_iperf_register_cmd(void);
void board_cli_init(utask_t *task)
{
    cli_service_init(task);

    cli_reg_cmd_help();
    cli_reg_cmd_ping();
    cli_reg_cmd_ifconfig();

#if defined(CONFIG_NTP)
    cli_reg_cmd_ntp();
#endif
    cli_reg_cmd_ps();
    cli_reg_cmd_free();
    cli_reg_cmd_factory();
    cli_reg_cmd_sysinfo();
    cli_reg_cmd_kvtool();
    cli_reg_cmd_sleep();
    cli_reg_cmd_m6131_passthrough();
    cli_reg_cmd_iperf();
}
