/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */
#if defined(CONFIG_BT_BREDR) && (CONFIG_BT_BREDR == 1)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <aos/cli.h>
#include <aos/bt.h>

static void cmd_bt_func(char *wbuf, int wbuf_len, int argc, char **argv)
{
    if (argc == 2) {
        if (strcmp(argv[1], "getaddr") == 0) {
            bt_dev_addr_t addr;
            ble_stack_get_local_addr(&addr);
            printf("addr=%02x:%02x:%02x:%02x:%02x:%02x\r\n",
                   addr.val[0],
                   addr.val[1],
                   addr.val[2],
                   addr.val[3],
                   addr.val[4],
                   addr.val[5]);
        }
    } else if (argc == 3) {
        if (strcmp(argv[1], "setname") == 0) {
            bt_stack_set_name(argv[2]);
        }
    } else {
        ;
    }
}

void cli_reg_cmd_bt(void)
{
    static const struct cli_command cmd_info = { "bt", "bt test", cmd_bt_func };

    aos_cli_register_command(&cmd_info);
}
#endif
