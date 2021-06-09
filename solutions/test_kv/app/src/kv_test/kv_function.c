/*
 * Copyright (C) 2019-2030 Alibaba Group Holding Limited
 */

#include <app_config.h>
#include "../yunit.h"

#define TAG "kv"

extern void __kv_dump();
extern void __show_data();
extern void __kv_list();

char * key,*value;
int ret;
char getvalue[497];
int intvalue;
float floatvalue;

static void function_kv_test(char *wbuf, int wbuf_len, int argc, char **argv) {
    if (argc == 2){
        if(strcmp(argv[1], "reset") == 0) {
            ret = aos_kv_reset();
            if (ret < 0){
                LOGE(TAG, "kv reset failed");
            } else {
                LOGI(TAG, "kv reset success");
            }
        } else if (strcmp(argv[1], "dump") == 0) {
            __kv_dump();
        } else if (strcmp(argv[1], "show") == 0) {           
            __show_data();
        } else if (strcmp(argv[1], "list") == 0) {           
            __kv_list();
        } else if (strcmp(argv[1], "perf") == 0) {           
            /*get or set*/
            uint32_t ts1, ts2, ts7, ts8;
            int ret;
            int count = 1;
            uint32_t ms_set_cost = 0;
            uint32_t ms_get1_cost = 0;
            uint32_t ms_set_cost_old = 0;
            uint32_t ms_get1_cost_old = 0;
            while(count <= 380) {
                char *test_data_key = malloc(16);
                snprintf(test_data_key, 16, "mytestkey%06d", count);
                char *test_data_value = malloc(6);
                snprintf(test_data_value, 6, "%05d", count);
                /*fresh set*/
                ts1 = aos_now_ms();
                ret = aos_kv_setstring(test_data_key, test_data_value);
                if (ret < 0) {
                    LOGE(TAG, "kv set ret:%d", ret);
                    break;
                }
                ts2 = aos_now_ms();
                ms_set_cost += ts2-ts1;

                /*get when exist*/
                ts7 = aos_now_ms();
                ret = aos_kv_getstring(test_data_key, getvalue, sizeof(getvalue));
                if (ret < 0) {
                    LOGE(TAG, "kv get ret:%d", ret);
                    break;
                }
                ts8 = aos_now_ms();
                ms_get1_cost += ts8-ts7;
                count++;
                if(count%76 == 0) {
                    LOGI(TAG, "run %d iterations: average set througput is %f Kps, get is %f Kps", count, 22000.0*(ms_set_cost-ms_set_cost_old)/(76*1024),  22000.0*(ms_get1_cost-ms_get1_cost_old)/(76*1024));
                    ms_set_cost_old = ms_set_cost;
                    ms_get1_cost_old = ms_get1_cost;
                }
            }
        } else {
            printf("Usage:\tkv set|setstr|setint|setfloat key value\n\tkv get|setstr|setint|getfloat key\n\tkv del key\n\tkv reset|dump|show|list\n");
	        return;
        }
        return;
    }

    if (argc < 3) {
	    printf("Usage:\tkv set|setstr|setint|setfloat key value\n\tkv get|setstr|setint|getfloat key\n\tkv del key\n\tkv reset|dump|show|list\n");
	    return;
    }
    key = argv[2];

    if (strcmp(argv[1], "set") == 0) {
        value = argv[3];
        ret = aos_kv_set(key, value, strlen(value), 1);
        if (ret < 0){
            LOGE(TAG, "kv set failed");
        } else {
            LOGI(TAG, "kv set success");
        }
    } else if (strcmp(argv[1], "setstr") == 0) {
        value = argv[3];
        ret = aos_kv_setstring(key, value);
        if (ret < 0){
            LOGE(TAG, "kv set string failed");
        } else {
            LOGI(TAG, "kv set string success");
        }
    } else if (strcmp(argv[1], "setint") == 0) {
        value = argv[3];
        ret = aos_kv_setint(key, atoi(value));
        if (ret < 0){
            LOGE(TAG, "kv set int failed");
        } else {
            LOGI(TAG, "kv set int success");
        }
    } else if (strcmp(argv[1], "setfloat") == 0) {
        value = argv[3];
        ret = aos_kv_setfloat(key, atof(value));
        if (ret < 0){
            LOGE(TAG, "kv set float failed");
        } else {
            LOGI(TAG, "kv set float success");
        }
    } else if (strcmp(argv[1], "get") == 0) {
        int value_len = sizeof(getvalue);
        memset(getvalue, 0, sizeof(getvalue));
        ret = aos_kv_get(key, getvalue, &value_len);
        if (ret < 0){
            LOGE(TAG, "kv get failed");
        } else {
            LOGI(TAG, "kv get %s", getvalue);
        }
    } else if (strcmp(argv[1], "getstr") == 0) {
        memset(getvalue, 0, sizeof(getvalue));
        ret = aos_kv_getstring(key, getvalue, sizeof(getvalue));
        if (ret < 0){
            LOGE(TAG, "kv get string failed");
        } else {
            LOGI(TAG, "kv get string %s", getvalue);
        }
    } else if (strcmp(argv[1], "getint") == 0) {
        ret = aos_kv_getint(key, &intvalue);
        if (ret < 0) {
            LOGE(TAG, "kv get int failed");
        } else {
            LOGI(TAG, "kv get int %d", intvalue);
        }
    } else if (strcmp(argv[1], "getfloat") == 0) {
        ret = aos_kv_getfloat(key, &floatvalue);
        if (ret < 0) {
            LOGE(TAG, "kv get float failed");
        } else {
            LOGI(TAG, "kv get float %f", floatvalue);
        }
    }else if (strcmp(argv[1], "del") == 0) {
        ret = aos_kv_del(key);
        if (ret < 0){
            LOGE(TAG, "kv del failed");
        } else {
            LOGI(TAG, "kv del success");
        }
    } else {
        printf("Usage:\tkv set|setstr|setint|setfloat key value\n\tkv get|setstr|setint|getfloat key\n\tkv del key\n\tkv reset|dump|show|list\n"); 
    }
}
void cli_reg_cmd_kvtest(void)
{
    static struct cli_command cmd_info = {
        "kv",
        "kv test commands",
        function_kv_test
    };
    aos_cli_register_command(&cmd_info);
}
