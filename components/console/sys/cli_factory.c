/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <yoc_config.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <yoc/nvram.h>
#include <aos/cli.h>
#include <drv/tee.h>

#define VALUE_SIZE    256

#define HELP_INFO "\n\tfactory set key [value]\n\tfactory get key\n"

static unsigned short fac_checksum(unsigned short *buf, int nword)
{
    unsigned long sum;

    for (sum = 0; nword > 0; nword--) {
        sum += *buf++;
    }

    sum  = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);

    return ~sum;
}

static void cmd_test_func(char *wbuf, int wbuf_len, int argc, char **argv)
{
    int ret;

    if (argc == 1) {
        printf("%s\n", HELP_INFO);
        return;
    }

    if (0 == strcmp(argv[1], "set")) {
        char *key = argv[2];
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
            //printf("ret: %d\n", ret);
            printf("%s\n", ret < 0 ? "failed" : "success");
        }
    } else if (0 == strcmp(argv[1], "get")) {
        char *key = argv[2];
        char res[VALUE_SIZE];

        if (!key) {
            printf("%s\n", HELP_INFO);
            return;
        }

        memset(res, 0, sizeof(res));
        ret = nvram_get_val(key, (char *)res, sizeof(res));
        printf("%s\n", res);
    } else if (0 == strcmp(argv[1], "setali")) {
        if (argc == 2) {
            nvram_set_val("PRODUCTKEY", NULL);
            nvram_set_val("DEVICENAME", NULL);
            nvram_set_val("DEVICESECRET", NULL);
            //printf("clear ali param\n");
        } else if (argc >= 5) {
            ret = nvram_set_val("PRODUCTKEY", argv[2]);

            //printf("PRODUCTKEY:%s ret: %d\n", argv[2], ret);
            if (ret > 0) {
                ret = nvram_set_val("DEVICENAME", argv[3]);
                //printf("DEVICENAME:%s ret: %d\n", argv[3], ret);
            }

            if (ret > 0) {
                ret = nvram_set_val("DEVICESECRET", argv[4]);
                //printf("DEVICESECRET:%s ret: %d\n", argv[4], ret);
            }

            /* do checksum */
            if (argc == 6 && ret > 0) {
                char item_buf[120];
                char cat_buf[240] = {0};
                int cat_len = 0;
                memset(cat_buf, 0, sizeof(cat_buf));
                ret = nvram_get_val("PRODUCTKEY", item_buf, sizeof(item_buf));

                if (ret > 0) {
                    if (ret < sizeof(cat_buf) - cat_len - 1) {
                        ret = ret < sizeof(item_buf) ? ret : sizeof(item_buf) - 1;
                        memcpy(&cat_buf[cat_len], item_buf, ret);
                        cat_len += ret;
                        ret = nvram_get_val("DEVICENAME", item_buf, sizeof(item_buf));
                    } else {
                        ret = -1;
                    }
                }

                if (ret > 0) {
                    if (ret < sizeof(cat_buf) - cat_len - 1) {
                        ret = ret < sizeof(item_buf) ? ret : sizeof(item_buf) - 1;
                        memcpy(&cat_buf[cat_len], item_buf, ret);
                        cat_len += ret;
                        ret = nvram_get_val("DEVICESECRET", item_buf, sizeof(item_buf));
                    } else {
                        ret = -1;
                    }
                }

                if (ret > 0) {
                    if (ret < sizeof(cat_buf) - cat_len - 1) {
                        ret = ret < sizeof(item_buf) ? ret : sizeof(item_buf) - 1;
                        memcpy(&cat_buf[cat_len], item_buf, ret);
                        cat_len += ret;
                        /*check sum*/
                        char *endptr;
                        unsigned short sum = fac_checksum((unsigned short *)cat_buf, (cat_len + 1) / 2);
                        long cmd_sum = strtol(argv[5], &endptr, 16);
                        //printf("checksum:%x %x\n", sum, cmd_sum);

                        if (sum != cmd_sum) {
                            ret = -1;
                        }
                    } else {
                        ret = -1;
                    }
                }

                //printf("%s\n", cat_buf);
            } else if (argc > 6){
                ret = -1;
            }

            printf("%s %s\n", argv[1], ret > 0 ? "success" : "failed");
        }  else {
            printf("%s\n", HELP_INFO);
        }
    } else if (0 == strcmp(argv[1], "getali")) {
        char item_buf[120];
        ret = nvram_get_val("PRODUCTKEY", item_buf, sizeof(item_buf));
        if (ret > 0) {
            printf("%s=%s\n", "PRODUCTKEY", item_buf);
        }
        ret = nvram_get_val("DEVICENAME", item_buf, sizeof(item_buf));
        if (ret > 0) {
            printf("%s=%s\n", "DEVICENAME", item_buf);
        }
        ret = nvram_get_val("DEVICESECRET", item_buf, sizeof(item_buf));
        if (ret > 0) {
            printf("%s=%s\n", "DEVICESECRET", item_buf);
        }
        //printf("ret=%d\n", ret);
    } else {
        printf("%s\n", HELP_INFO);
    }
}

void cli_reg_cmd_factory(void)
{
    static const struct cli_command cmd_info = {
        "factory",
        "set/get factory info",
        cmd_test_func
    };

    aos_cli_register_command(&cmd_info);
}
