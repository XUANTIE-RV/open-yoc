/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */
#include <aos/debug.h>
#include <aos/cli.h>
#include <aos/kv.h>
#include <aos/kernel.h>

extern void cli_reg_cmd_ping(void);
extern void cli_reg_cmd_ifconfig(void);
extern void cli_reg_cmd_kvtool(void);

//#define CLI_CMD_DEBUG

void board_cli_init()
{
    aos_cli_init();

    extern void cli_reg_cmd_ps(void);
    cli_reg_cmd_ps();

    extern void cli_reg_cmd_sysinfo(void);
    cli_reg_cmd_sysinfo();

    extern void cli_reg_cmd_free(void);
    cli_reg_cmd_free();

    cli_reg_cmd_kvtool();

#ifdef CLI_CMD_DEBUG
    extern void cli_reg_cmd_ble(void);
    cli_reg_cmd_ble();

    extern void cli_reg_cmd_blemesh(void);
    cli_reg_cmd_blemesh();

    extern void cli_reg_cmd_bt(void);
    cli_reg_cmd_bt();

    int ble_debug = 0;
    int ret;
    ret = aos_kv_getint("ble_debug", &ble_debug);
    if (ret < 0) {
        ble_debug = 0;
    }

    if (ble_debug) {
        LOGI("Init", "BLE Debug Mode, type `ble` or `blemesh` for support command");
        while(1)
        {
            aos_msleep(100000);
        }
    }
#endif
}
