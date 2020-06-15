/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <stdlib.h>

#include <aos/kv.h>
#include <aos/cli.h>
#include <yoc/partition.h>

#define HELP_INFO "Usage:\tkv set key value\n\tkv get key\n\tkv setint key value\n\tkv getint key\n\tkv del key\n"

static void cmd_kvfs_func(char *wbuf, int wbuf_len, int argc, char **argv)
{
    char * key,*value;
    int ret;
    char getvalue[128];
    int intvalue;

#if defined(CONFIG_DEBUG)
    if (argc == 2) {
        if (strcmp(argv[1], "dump") == 0) {
            extern void __kv_dump();
            __kv_dump();
        } else if (strcmp(argv[1], "raw") == 0) {
            extern void __show_data();
            __show_data();

        } else if (strcmp(argv[1], "list") == 0) {
            extern void __kv_list();
            __kv_list();
        } else if (strcmp(argv[1], "erase") == 0) {
            partition_t h = partition_open("kv");
            partition_erase(h, 0, 4);
            partition_close(h);
        }

        return;
    }
#endif

    if (argc < 3) {
        printf("%s\n", HELP_INFO);
        return;
    }

    key = argv[2];

    if (0 == strcmp(argv[1], "set") ){
        if (argc == 3) {
            ret = aos_kv_del(key);
            printf("kv del %s\n", ret < 0 ? "failed" : "success");
        } else  {
            value = argv[3];
            ret = aos_kv_setstring(key, value);
            printf("kv set string %s\n", ret < 0 ? "failed" : "success");
        }

    } else if (0 == strcmp(argv[1], "get") ) {
        memset(getvalue, 0, sizeof(getvalue));
        ret = aos_kv_getstring(key, getvalue, sizeof(getvalue));
        printf("kv get %s\n", ret < 0 ? "failed" : getvalue);
    } else if (0 == strcmp(argv[1], "del") ) {
        ret = aos_kv_del(key);
        printf("kv del %s\n", ret < 0 ? "failed" : "success");
    } else if (0 == strcmp(argv[1], "setint") ){
        if (argc == 3) {
            ret = aos_kv_del(key);
            printf("kv del %s\n", ret < 0 ? "failed" : "success");
        } else  {
            value = argv[3];
            ret = aos_kv_setint(key, atoi(value));
            printf("kv set int %s\n", ret < 0 ? "failed" : "success");
        }

    } else if (0 == strcmp(argv[1], "getint") ) {
        ret = aos_kv_getint(key, &intvalue);
        if (ret >= 0) {
            printf("kv getint %d\n", intvalue);
        } else {
            printf("kv getint fail\n");
        }
    }
}

void cli_reg_cmd_kvtool(void)
{
    static const struct cli_command cmd_info = 
    {
        "kv",
        "kv tools",
        cmd_kvfs_func
    };

    aos_cli_register_command(&cmd_info);
}
