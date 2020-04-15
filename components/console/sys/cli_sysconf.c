/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <yoc_config.h>

#include <aos/cli.h>
#include <aos/version.h>

#if defined(CONFIG_AMT)
#include "amt.h"
#endif

#ifdef CONFIG_CHIP_ZX297100

#include <oss_nv.h>

#define BOOT_FLAG_BASE      (0x1003FE00)


#define HELP_INFO \
    "Usage: sysconf <command> [option]\n" \
    "<command>:\n" \
    "dlopen : open or close download port\n" \
    "dlclose : close download port\n" \
    "dl : get download port status\n"

static void cmd_sysconf_func(char *wbuf, int wbuf_len, int argc, char **argv)
{
    uint8_t dl_flag;
    int ret = 0;

    if (1 >= argc) {
        printf(HELP_INFO);
        return;
    }

    if (0 == strcmp(argv[1], "dl")) {
#if defined(CONFIG_AMT)
        ret = amt_eflash_read(BOOT_FLAG_BASE, (u8 *)(&dl_flag), 1);
#else
        ret = -1;
#endif
        if (ret < 0) {
            printf("read download port status fail, ret %d\n", ret);
            return;
        }

        printf("download port: %s\n", dl_flag == 0 ? "open" : "close");
    } else if (0 == strcmp(argv[1], "dlopen")) {
        dl_flag = 0;
#if defined(CONFIG_AMT)
        ret = amt_eflash_write(BOOT_FLAG_BASE, (u8 *)(&dl_flag), 1);
#else
        ret = -1;
#endif
        if (ret < 0) {
            printf("open download port fail, ret %d\n", ret);
            return;
        }

        printf("download port: open\n");
    } else if (0 == strcmp(argv[1], "dlclose")) {
        dl_flag = 1;
#if defined(CONFIG_AMT)
        ret = amt_eflash_write(BOOT_FLAG_BASE, (u8 *)(&dl_flag), 1);
#else
                ret = -1;
#endif
        if (ret < 0) {
            printf("open download port fail, ret %d\n", ret);
            return;
        }

        printf("download port: close\n");
    } else {
        printf(HELP_INFO);
    }
}

void cli_reg_cmd_sysconf(void)
{
    static const struct cli_command cmd_info = {
        "sysconf",
        "sysconf : os config, type sysconf help for more info",
        cmd_sysconf_func,
    };

    aos_cli_register_command(&cmd_info);
}

#endif
