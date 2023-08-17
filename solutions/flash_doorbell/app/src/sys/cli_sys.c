/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>

#include <aos/cli.h>
#include <aos/debug.h>

#include <drv/tick.h>
#include <ntp.h>

#include "app_sys.h"

static void cmd_appsys_func(char *wbuf, int wbuf_len, int argc, char **argv)
{
    int item_count = argc;

    if (item_count == 2) {
        if (strcmp(argv[1], "us") == 0) {
            uint64_t us = csi_tick_get_us();
            printf("\ttick us: %llu\n", (long long unsigned)us);
        } else if (strcmp(argv[1], "time") == 0) {
            time_t t   = time(NULL);
            time_t lct = t + timezone * 3600;
            if (t >= 0) {
                printf("\tTZ(%02ld): %s %lld\n", timezone, ctime(&t), (long long)lct);
                printf("\t   UTC: %s %lld\n", asctime(gmtime(&t)), (long long)t);
            }
        } else if (strcmp(argv[1], "assert") == 0) {
            printf("start aos_assert test\r\n");
            aos_assert(0);
        } else if (strcmp(argv[1], "assert2") == 0) {
            printf("start assert test\r\n");
            assert(0);
        } else if (strcmp(argv[1], "abort") == 0) {
            printf("start abort test\r\n");
            abort();
        } else {
            ;
        }
    } else if (item_count == 3) {
        if (strcmp(argv[1], "ntp") == 0) {
            ntp_sync_time(argv[2]);
        } else if (strcmp(argv[1], "crash") == 0) {
            int type = atoi(argv[2]);
            switch (type) {
                case 0: {
                    int *nullprt = NULL;
                    *nullprt     = 1;
                    break;
                }
                case 1: {
                    typedef void (*func_ptr_t)();
                    func_ptr_t f = (func_ptr_t)0x12345678;
                    f();
                    break;
                }
                case 2: {
                    int a = 100;
                    int b = 0;
                    int c = a / b;
                    printf("a/b=%d\r\n", c);
                    break;
                }
                default:;
            }
        } else if (strcmp(argv[1], "wdt") == 0) {
            int type = atoi(argv[2]);
            app_sys_except_init(type);
        }
    } else {
        ;
    }
}

void cli_reg_cmd_appsys(void)
{
    static const struct cli_command cmd_info = { "appsys", "app extend command.", cmd_appsys_func };

    aos_cli_register_command(&cmd_info);
}
