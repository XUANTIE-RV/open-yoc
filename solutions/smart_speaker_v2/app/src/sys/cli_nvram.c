/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <yoc/nvram.h>
#include <aos/cli.h>
#include <drv/tee.h>

#define VALUE_SIZE 256

#define HELP_INFO "\n\tnvram set key [value]\n\tnvram get key\n"

static void cmd_nvram_func(char *wbuf, int wbuf_len, int argc, char **argv)
{
    int ret;

    if (argc == 1) {
        printf("%s\n", HELP_INFO);
        return;
    }

    if (0 == strcmp(argv[1], "set")) {
        char *key   = argv[2];
        char *value = argv[3];

        if (!key) {
            printf("%s\n", HELP_INFO);
            return;
        }

        if (!value) {
            char res[VALUE_SIZE];
            memset(res, 0, sizeof(res));
            ret = nvram_set_val(key, NULL);
            printf("%s\n", res);
        } else {
            ret = nvram_set_val(key, value);
            // printf("ret: %d\n", ret);
            printf("%s\n", ret < 0 ? "failed" : "success");
        }
    } else if (0 == strcmp(argv[1], "get")) {
        char *key = argv[2];
        char  res[VALUE_SIZE];

        if (!key) {
            printf("%s\n", HELP_INFO);
            return;
        }

        memset(res, 0, sizeof(res));
        ret = nvram_get_val(key, (char *)res, sizeof(res));
        printf("%s\n", res);
    } else {
        printf("%s\n", HELP_INFO);
    }
}

void cli_reg_cmd_nvram(void)
{
    static const struct cli_command cmd_info = { "nvram", "set/get nvram info", cmd_nvram_func };

    aos_cli_register_command(&cmd_info);
}
