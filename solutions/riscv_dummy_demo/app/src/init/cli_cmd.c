/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */


#include <aos/debug.h>
#include <aos/cli.h>

void cmd_qemu_quit(char *wbuf, int wbuf_len, int argc, char **argv)
{
    if (argc > 0) {
        if (!strcmp(argv[0], "q") || !strcmp(argv[0], "quit")) {
            aos_cli_printf("quit from qemu.\n");
            *(unsigned long *)0x10002000 = 1;
            return;
        }
    }
    aos_cli_printf("Usage: quit\n");
}

void cli_reg_cmd_quit(void)
{
    static const struct cli_command cmd_info = {
        "quit",
        "quit from qemu",
        cmd_qemu_quit
    };

    aos_cli_register_command(&cmd_info);
}

void board_cli_init()
{
    aos_cli_init();
    cli_reg_cmd_quit();
}
