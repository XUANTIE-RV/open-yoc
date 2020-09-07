/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <string.h>

#include <aos/cli.h>
#include <aos/kernel.h>
#include <yoc/sysinfo.h>

#define HELP_INFO \
    "Usage: sys <command>\n" \
    "\tos: show os version\n" \
    "\tapp: show app version\n" \
    "\tid: show device id\n" \
    "\treboot: reboot sys\n"

static void cmd_get_sysinfo_func(char *wbuf, int wbuf_len, int argc, char **argv)
{
    if (argc > 1) {
        if (0 == strcmp(argv[1], "os")) {
            printf("%s\n", aos_get_os_version());
            return;
        } else if (0 == strcmp(argv[1], "id")) {
            printf("%s\n", aos_get_device_id());
            return;
        } else if (0 == strcmp(argv[1], "app")) {
            printf("%s\n", aos_get_app_version());
            return;
        } else if (0 == strcmp(argv[1], "reboot")) {
            aos_reboot();
            return;
        } else if (0 == strcmp(argv[1], "sleep")) {
            if (argc == 3 && argv[2]) {
                aos_msleep(atoi(argv[2]) * 1000);
                return;
            }
        } else if (0 == strcmp(argv[1], "crash")) {
            int *ptr = (int *)0x7f000001;
            *ptr = 1;
            return;
        }
    }

    printf(HELP_INFO);
}

void cli_reg_cmd_sysinfo(void)
{
    static const struct cli_command cmd_info = {
        "sys",
        "sys comand",
        cmd_get_sysinfo_func,
    };

    aos_cli_register_command(&cmd_info);
}
