/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <aos/aos.h>
#include <app_init.h>
#include <aos/cli.h>
#include <api/mesh.h>
#include "mesh_ctrl.h"

static const char *TAG = "Mesh CLI";

#define MESH_NODE_APP_CLICMD_COUNT 2

void cmd_mesh_control_func(char *wbuf, int wbuf_len, int argc, char **argv)
{
    uint8_t ack, onoff;
    int ret;

    if (argc < 3) {
        LOGE(TAG, "usage: meshonoff onoff ack\n");
        return ;
    }

    onoff = atoi(argv[1]);
    ack = atoi(argv[2]);

    if (ack == 0) {
        ret = gen_onoff_set(onoff, false);

        if (ret) {
            LOGE(TAG, "send unack msg LED faild");
        }
    } else if (ack == 1) {
        ret = gen_onoff_set(onoff, true);

        if (ret) {
            LOGE(TAG, "send ack msg LED faild");
        }
    }
}

void cmd_mesh_reset_func(char *wbuf, int wbuf_len, int argc, char **argv)
{
    LOGD(TAG, "Mesh Node Reset");
    bt_mesh_reset();
}

void cli_reg_cmd_switch_ctrl(void)
{
    static const struct cli_command cmd_info[MESH_NODE_APP_CLICMD_COUNT] = {
        {
            "meshonoff",
            "meshonoff <onoff 0,1> <ack 0,1>",
            cmd_mesh_control_func,
        },
        {
            "meshrst",
            "mesh reset",
            cmd_mesh_reset_func,
        },
    };

    aos_cli_register_commands(cmd_info, MESH_NODE_APP_CLICMD_COUNT);
}

