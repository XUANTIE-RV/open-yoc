/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <aos/cli.h>

#include "app_key_msg.h"

/*************************************************
 * 通过命令行模拟按键，测试按键消息
 *************************************************/
#ifdef CONFIG_DEBUG
static char *msgidstr[KEY_MSG_COUNT] = {
    "vol+", "vol-", "mute", "play", "user", "wifiprov", "bt", "power", "uplog", "fct", "volup_ld", "voldn_ld"
};
#endif

static void cmd_key_msg_func(char *wbuf, int wbuf_len, int argc, char **argv)
{
    if (argc == 2) {
        int keymsg_id = atoi(argv[1]);
        app_key_msg_send(keymsg_id, NULL);
    } else {
        printf("usage:keymsg msgid[0~%d]\r\n", KEY_MSG_COUNT - 1);
#ifdef CONFIG_DEBUG
        for (int i = 0; i < KEY_MSG_COUNT; i++) {
            printf("msgid=%d %s\r\n", i, msgidstr[i]);
        }
#endif
    }
}

void cli_reg_cmd_keymsg(void)
{
    static const struct cli_command cmd_info = { "keymsg", "key msg test", cmd_key_msg_func };

    aos_cli_register_command(&cmd_info);
}
