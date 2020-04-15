/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <aos/kv.h>
#include "at_internal.h"

#define TAG "at_kv_cmd"

#define MAX_KEY_SZ (32)
#define MAX_VAL_SZ (32)

void at_cmd_kv_set(char *cmd, int type, char *data)
{
    if (type == WRITE_CMD) {
        int key_cnt = MAX_KEY_SZ + 1;
        int val_cnt = MAX_VAL_SZ + 1;

        atserver_scanf("%*[^,]%n%*[^\0]%n", &key_cnt, &val_cnt);
        val_cnt -= (key_cnt + 1);
        if (key_cnt <= MAX_KEY_SZ && val_cnt <= MAX_VAL_SZ && val_cnt > 0) {
            char key[MAX_KEY_SZ + 1] = {'\0'};
            char val[MAX_VAL_SZ + 1] = {'\0'};

            if (atserver_scanf("%[^,],%[^\0]", key, val) == 2) {
                int ret = aos_kv_setstring(key, (const char *)val);

                if (ret == 0) {
                    AT_BACK_OK();
                    return;
                }
            }
        }

        AT_BACK_RET_ERR(cmd, AT_ERR);
    }
}

void at_cmd_kv_get(char *cmd, int type, char *data)
{
    if (type == WRITE_CMD) {
        if (strlen(data) <= MAX_KEY_SZ && strlen(data) > 0) {
            char res[MAX_VAL_SZ + 1] = {'\0'};
            int ret = aos_kv_getstring(data, (char *)res, MAX_VAL_SZ + 1);

            if (ret >= 0 && ret <= MAX_VAL_SZ) {
                AT_BACK_RET_OK(cmd, res);
                return;
            }
        }

        AT_BACK_RET_ERR(cmd, AT_ERR);
    }
}

void at_cmd_kv_del(char *cmd, int type, char *data)
{
    if (type == WRITE_CMD) {
        if (strlen(data) <= MAX_KEY_SZ && strlen(data) > 0) {
            int ret = aos_kv_del(data);

            if (ret == 0) {
                AT_BACK_OK();;
                return;
            }
        }

        AT_BACK_RET_ERR(cmd, AT_ERR);
    }
}

void at_cmd_kv_setint(char *cmd, int type, char *data)
{
    if (type == WRITE_CMD) {
        int key_cnt = MAX_KEY_SZ + 1;
        int val_cnt = MAX_VAL_SZ + 1;

        atserver_scanf("%*[^,]%n%*[^\0]%n", &key_cnt, &val_cnt);
        val_cnt -= (key_cnt + 1);

        if (key_cnt <= MAX_KEY_SZ && val_cnt <= MAX_VAL_SZ && val_cnt > 0) {
            char key[MAX_KEY_SZ + 1] = {'\0'};
            char val[MAX_VAL_SZ + 1] = {'\0'};
            if (atserver_scanf("%[^,],%[-0-9]", key, val) == 2) {
                if (strlen(val) == val_cnt) {
                    int ret = aos_kv_setint(key, atoi(val));

                    if (ret == 0) {
                        AT_BACK_OK();
                        return;
                    }
                }
            }
        }
        AT_BACK_RET_ERR(cmd, AT_ERR);
    }
}

void at_cmd_kv_getint(char *cmd, int type, char *data)
{
    if (type == WRITE_CMD) {
        if (strlen(data) <= MAX_KEY_SZ && strlen(data) > 0) {
            int value = 0;
            int ret = aos_kv_getint(data, &value);

            if (ret >= 0 && ret <= MAX_VAL_SZ) {
                AT_BACK_RET_OK_INT(cmd, value);
                return;
            }
        }

        AT_BACK_RET_ERR(cmd, AT_ERR);
    }
}

void at_cmd_kv_delint(char *cmd, int type, char *data)
{
    if (type == WRITE_CMD) {
        if (strlen(data) <= MAX_KEY_SZ && strlen(data) > 0) {
            int ret = aos_kv_del(data);

            if (ret == 0) {
                AT_BACK_OK();
                return;
            }
        }

        AT_BACK_RET_ERR(cmd, AT_ERR);
    }
}

