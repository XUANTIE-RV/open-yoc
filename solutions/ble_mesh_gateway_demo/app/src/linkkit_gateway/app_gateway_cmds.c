/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <aos/aos.h>
#include <aos/yloop.h>
//#include "netmgr.h"
#include "smartliving/iot_export.h"
#include "smartliving/iot_import.h"
#include "aos/kv.h"

#include <k_api.h>

#include "app_gateway_main.h"
#include "app_gateway_cmds.h"
#include "app_gateway_ut.h"

static char awss_running = 0;

static void awss_close_dev_ap(void *p)
{
    awss_dev_ap_stop();
    gateway_info("%s exit\n", __func__);
    aos_task_exit(0);
}

void gateway_open_dev_ap(void *p)
{
#if 0    
    iotx_event_regist_cb(user_event_monitor);
    gateway_info("%s\n", __func__);
    if (netmgr_start(false) != 0)
    {
        aos_msleep(2000);
        awss_dev_ap_start();
    }
    aos_task_exit(0);
#endif    
}

static void stop_netmgr(void *p)
{
    awss_stop();
    gateway_info("%s\n", __func__);
    aos_task_exit(0);
}

void gateway_start_netmgr(void *p)
{
#if 0    
    iotx_event_regist_cb(user_event_monitor);
    netmgr_start(true);
    aos_task_exit(0);
#endif    
}

void gateway_do_awss_active(void)
{
    gateway_info("gateway_do_awss_active %d\n", awss_running);
    awss_running = 1;
#ifdef WIFI_PROVISION_ENABLED
    extern int awss_config_press();
    awss_config_press();
#endif
}

void do_awss_dev_ap(void)
{
    aos_task_new("netmgr_stop", stop_netmgr, NULL, 4096);
    aos_task_new("dap_open", gateway_open_dev_ap, NULL, 4096);
}

void do_awss()
{
    aos_task_new("dap_close", awss_close_dev_ap, NULL, 2048);
    aos_task_new("netmgr_start", gateway_start_netmgr, NULL, 4096);
}

static void linkkit_reset(void *p)
{
    iotx_sdk_reset_local();
    HAL_Reboot();
}

//static iotx_vendor_dev_reset_type_t reset_type = IOTX_VENDOR_DEV_RESET_TYPE_UNBIND_SHADOW_CLEAR;

void gateway_awss_reset(void)
{
#ifdef WIFI_PROVISION_ENABLED
    aos_task_new("reset", (void (*)(void *))iotx_sdk_reset, (void *)&reset_type, 4096); // stack taken by iTLS is more than taken by TLS.
#endif
    aos_post_delayed_action(2000, linkkit_reset, NULL);
}

void gateway_awss_reset_with_type(iotx_vendor_dev_reset_type_t *reset_type)
{
#ifdef WIFI_PROVISION_ENABLED
    aos_task_new("reset", (void (*)(void *))iotx_sdk_reset, (void *)reset_type, 4096); // stack taken by iTLS is more than taken by TLS.
#endif
    aos_post_delayed_action(2000, linkkit_reset, NULL);
}

#ifdef CONFIG_AOS_CLI
static void handle_reset_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    int reset_type = -1;

    if (argc == 2)
    {
        reset_type = atoi(argv[1]);

        printf("reset-type:%d\n", reset_type);
        if (reset_type >= 0)
        {
            gateway_awss_reset_with_type(&reset_type);
        }
    }
    else
    {
        aos_schedule_call(gateway_awss_reset, NULL);
    }
}

static void handle_active_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    aos_schedule_call(gateway_do_awss_active, NULL);
}

static void handle_dev_ap_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    aos_schedule_call(do_awss_dev_ap, NULL);
}

static void handle_awss_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    aos_schedule_call(do_awss, NULL);
}

#if defined(CONFIG_AOS_CLI) && defined(OFFLINE_LOG_UT_TEST)
static void handle_offline_log_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    int ret = 0;

    if (argc < 2)
    {
        aos_cli_printf("params err,oll [save/upload/erase] or [read [0-1]] or [write [0-3] \"test offline log\"]\r\n");
        return;
    }

    aos_cli_printf("cmd:%s\r\n", argv[1]);

    if (!strcmp("save", argv[1]) && argc == 2)
    {
        diagnosis_offline_log_save_all();
    }
    else if (!strcmp("upload", argv[1]) && argc == 4)
    {
        int port = atoi(argv[3]);

        diagnosis_offline_log_upload(argv[2], port);
    }
    else if (!strcmp("write", argv[1]) && argc == 4)
    {
        int loglevel = atoi(argv[2]);

        if (loglevel >= 0 && loglevel <= 3)
        {
            diagnosis_offline_log(loglevel, "%s\\n", argv[3]);
        }
    }
    else if (!strcmp("read", argv[1]) && argc == 3)
    {
        int log_mode = atoi(argv[2]);

        diagnosis_offline_log_read_all(log_mode);
    }
    else if (!strcmp("erase", argv[1]) && argc == 2)
    {
        diagnosis_offline_log_erase_flash_desc();
    }
    else
    {
        aos_cli_printf("cmd:%s is not support\r\n", argv[1]);
    }

    if (ret != 0)
    {
        aos_cli_printf("handle oll cmd:%s failed:%d\r\n", argv[1], ret);
    }

    return -1;
}

static struct cli_command offline_log_cmd = {.name = "oll",
                                             .help = "oll [save]/[erase] or [upload host port] or [read [0-1]] or [write [0-3] \"test offline log\"]",
                                             .function = handle_offline_log_cmd};

#endif

static void handle_linkkey_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    int index = 0;
    char key_buf[MAX_KEY_LEN];

    if (argc == 1)
    {
        int len = 0;
        char product_key[PRODUCT_KEY_LEN + 1] = {0};
        char product_secret[PRODUCT_SECRET_LEN + 1] = {0};
        char device_name[DEVICE_NAME_LEN + 1] = {0};
        char device_secret[DEVICE_SECRET_LEN + 1] = {0};

        for (index = 0; index < MAX_DEVICES_META_NUM; index++)
        {
            len = PRODUCT_KEY_LEN + 1;
            memset(key_buf, 0, MAX_KEY_LEN);
            memset(product_key, 0, sizeof(product_key));
            HAL_Snprintf(key_buf, MAX_KEY_LEN, "%s_%d", KV_KEY_PK, index);
            aos_kv_get(key_buf, product_key, &len);

            len = PRODUCT_SECRET_LEN + 1;
            memset(key_buf, 0, MAX_KEY_LEN);
            memset(product_secret, 0, sizeof(product_secret));
            HAL_Snprintf(key_buf, MAX_KEY_LEN, "%s_%d", KV_KEY_PS, index);
            aos_kv_get(key_buf, product_secret, &len);

            len = DEVICE_NAME_LEN + 1;
            memset(key_buf, 0, MAX_KEY_LEN);
            memset(device_name, 0, sizeof(device_name));
            HAL_Snprintf(key_buf, MAX_KEY_LEN, "%s_%d", KV_KEY_DN, index);
            aos_kv_get(key_buf, device_name, &len);

            len = DEVICE_SECRET_LEN + 1;
            memset(key_buf, 0, MAX_KEY_LEN);
            memset(device_secret, 0, sizeof(device_secret));
            HAL_Snprintf(key_buf, MAX_KEY_LEN, "%s_%d", KV_KEY_DS, index);
            aos_kv_get(key_buf, device_secret, &len);

            if (index == 0)
            {
                aos_cli_printf("\r\nMaster Dev:\r\n");
                aos_cli_printf("PK=%s.\r\n", product_key);
                aos_cli_printf("PS=%s.\r\n", product_secret);
                aos_cli_printf("DN=%s.\r\n", device_name);
                aos_cli_printf("DS=%s.\r\n", device_secret);
            }
            else
            {
                aos_cli_printf("\r\nSubDev(%d):\r\n", index);
                aos_cli_printf("PK=%s.\r\n", product_key);
                aos_cli_printf("PS=%s.\r\n", product_secret);
                aos_cli_printf("DN=%s.\r\n", device_name);
                aos_cli_printf("DS=%s.\r\n", device_secret);
            }
        }
    }
    else if (argc == 6)
    {
        int devid = atoi(argv[1]);

        if (devid >= 0 && devid <= MAX_DEVICES_META_NUM)
        {
            memset(key_buf, 0, MAX_KEY_LEN);
            HAL_Snprintf(key_buf, MAX_KEY_LEN, "%s_%d", KV_KEY_PK, devid);
            aos_kv_set(key_buf, argv[2], strlen(argv[2]) + 1, 1);

            memset(key_buf, 0, MAX_KEY_LEN);
            HAL_Snprintf(key_buf, MAX_KEY_LEN, "%s_%d", KV_KEY_DN, devid);
            aos_kv_set(key_buf, argv[3], strlen(argv[3]) + 1, 1);

            memset(key_buf, 0, MAX_KEY_LEN);
            HAL_Snprintf(key_buf, MAX_KEY_LEN, "%s_%d", KV_KEY_DS, devid);
            aos_kv_set(key_buf, argv[4], strlen(argv[4]) + 1, 1);

            memset(key_buf, 0, MAX_KEY_LEN);
            HAL_Snprintf(key_buf, MAX_KEY_LEN, "%s_%d", KV_KEY_PS, devid);
            aos_kv_set(key_buf, argv[5], strlen(argv[5]) + 1, 1);

            aos_cli_printf("Done\r\n");
        }
        else
        {
            aos_cli_printf("Err:devid range[0-%d]", MAX_DEVICES_META_NUM - 1);
        }
    }
    else
    {
        aos_cli_printf("Params Err\r\n");
    }

    return;
}

//This func just to simulate gateway operate subdev
void handle_gw_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    gateway_msg_t msg;
    int ret = 0;

    if (argc < 2)
    {
        gateway_err("params err,gw [add/del/reset/queryid subdevid] or gw [addall/update]");
        return;
    }

    gateway_info("cmd:%s", argv[1]);

    if (!strcmp("add", argv[1]) && argc == 3)
    {
        int devid = atoi(argv[2]);

        if (devid > 0 && devid < MAX_DEVICES_META_NUM)
        {
            msg.msg_type = GATEWAY_MSG_TYPE_ADD;
            msg.devid = devid;

            ret = app_gateway_ut_send_msg(&msg, sizeof(gateway_msg_t));
        }
        else
        {
            gateway_warn("devid should in range[1-%d]", MAX_DEVICES_META_NUM - 1);
        }
    }
    else if (!strcmp("addr", argv[1]) && argc == 4)
    {
        int devid = atoi(argv[2]);
        int devid_end = atoi(argv[3]);

        if (devid > 0 && devid < MAX_DEVICES_META_NUM &&
            devid_end > 0 && devid_end < MAX_DEVICES_META_NUM &&
            devid < devid_end)
        {
            msg.msg_type = GATEWAY_MSG_TYPE_ADD_RANGE;
            msg.devid = devid;
            msg.devid_end = devid_end;

            ret = app_gateway_ut_send_msg(&msg, sizeof(gateway_msg_t));
        }
        else
        {
            gateway_warn("devid should in range[1-%d]", MAX_DEVICES_META_NUM - 1);
        }
    }
    else if (!strcmp("del", argv[1]) && argc == 3)
    {
        int devid = atoi(argv[2]);

        if (devid > 0 && devid < MAX_DEVICES_META_NUM)
        {
            msg.msg_type = GATEWAY_MSG_TYPE_DEL;
            msg.devid = devid;

            ret = app_gateway_ut_send_msg(&msg, sizeof(gateway_msg_t));
        }
        else
        {
            gateway_warn("devid should in range[1-%d]", MAX_DEVICES_META_NUM - 1);
        }
    }
    else if (!strcmp("delr", argv[1]) && argc == 4)
    {
        int devid = atoi(argv[2]);
        int devid_end = atoi(argv[3]);

        if (devid > 0 && devid < MAX_DEVICES_META_NUM &&
            devid_end > 0 && devid_end < MAX_DEVICES_META_NUM &&
            devid < devid_end)
        {
            msg.msg_type = GATEWAY_MSG_TYPE_DEL_RANGE;
            msg.devid = devid;
            msg.devid_end = devid_end;

            ret = app_gateway_ut_send_msg(&msg, sizeof(gateway_msg_t));
        }
        else
        {
            gateway_warn("devid should in range[1-%d]", MAX_DEVICES_META_NUM - 1);
        }
    }
    else if (!strcmp("addall", argv[1]) && argc == 2)
    {
        msg.msg_type = GATEWAY_MSG_TYPE_ADDALL;
        msg.devid = 1;

        ret = app_gateway_ut_send_msg(&msg, sizeof(gateway_msg_t));
    }
    else if (!strcmp("delall", argv[1]) && argc == 2)
    {
        msg.msg_type = GATEWAY_MSG_TYPE_DELALL;
        msg.devid = 1;

        ret = app_gateway_ut_send_msg(&msg, sizeof(gateway_msg_t));
    }
    else if (!strcmp("update", argv[1]) && argc == 2)
    {
        msg.msg_type = GATEWAY_MSG_TYPE_UPDATE;
        msg.devid = 1;

        ret = app_gateway_ut_send_msg(&msg, sizeof(gateway_msg_t));
    }
    else if (!strcmp("loginall", argv[1]) && argc == 2)
    {
        msg.msg_type = GATEWAY_MSG_TYPE_LOGIN_ALL;
        msg.devid = 1;

        ret = app_gateway_ut_send_msg(&msg, sizeof(gateway_msg_t));
    }
    else if (!strcmp("logoutall", argv[1]) && argc == 2)
    {
        msg.msg_type = GATEWAY_MSG_TYPE_LOGOUT_ALL;
        msg.devid = 1;

        ret = app_gateway_ut_send_msg(&msg, sizeof(gateway_msg_t));
    }
    else if (!strcmp("reset", argv[1]) && argc == 3)
    {
        int devid = atoi(argv[2]);

        if (devid > 0 && devid < MAX_DEVICES_META_NUM)
        {
            msg.msg_type = GATEWAY_MSG_TYPE_RESET;
            msg.devid = devid;

            ret = app_gateway_ut_send_msg(&msg, sizeof(gateway_msg_t));
        }
        else
        {
            gateway_warn("devid should in range[1-%d]", MAX_DEVICES_META_NUM - 1);
        }
    }
    else if (!strcmp("queryid", argv[1]) && argc == 3)
    {
        int devid = atoi(argv[2]);

        if (devid > 0 && devid < MAX_DEVICES_META_NUM)
        {
            msg.msg_type = GATEWAY_MSG_TYPE_QUERY_SUBDEV_ID;
            msg.devid = devid; //This devid is device in KV

            ret = app_gateway_ut_send_msg(&msg, sizeof(gateway_msg_t));
        }
        else
        {
            gateway_warn("devid should in range[1-%d]", MAX_DEVICES_META_NUM - 1);
        }
    }
    else
    {
        gateway_warn("cmd:%s is not support", argv[1]);
    }

    if (ret != 0)
    {
        gateway_err("handle gw cmd:%s failed:%d", argv[1], ret);
    }

    return -1;
}

#ifdef DEV_ERRCODE_ENABLE
#define DEV_DIAG_CLI_SET_ERRCODE "set_err"
#define DEV_DIAG_CLI_ERRCODE_MANUAL_MSG "errcode was manually set"
#define DEV_DIAG_CLI_GET_ERRCODE "get_err"
#define DEV_DIAG_CLI_DEL_ERRCODE "del_err"

static void handle_dev_serv_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    uint16_t err_code = 0;
    char err_msg[DEV_ERRCODE_MSG_MAX_LEN] = {0};
    uint8_t ret = 0;
    if (argc == 1)
    {
        aos_cli_printf("device diagnosis service test, please input more parameters.\r\n");
    }
    else if (argc > 1)
    {
        if ((argc == 3) && (!strcmp(argv[1], DEV_DIAG_CLI_SET_ERRCODE)))
        {
            err_code = (uint16_t)strtoul(argv[2], NULL, 0);
            ret = dev_errcode_kv_set(err_code, DEV_DIAG_CLI_ERRCODE_MANUAL_MSG);
            if (ret == 0)
            {
                aos_cli_printf("manually set err_code=%d\r\n", err_code);
            }
            else
            {
                aos_cli_printf("manually set err_code failed!\r\n");
            }
        }
        else if ((argc == 2) && (!strcmp(argv[1], DEV_DIAG_CLI_GET_ERRCODE)))
        {
            ret = dev_errcode_kv_get(&err_code, err_msg);
            if (ret == 0)
            {
                aos_cli_printf("get err_code=%d, err_msg=%s\r\n", err_code, err_msg);
            }
            else
            {
                aos_cli_printf("get err_code fail or no err_code found!\r\n");
            }
        }
        else if ((argc == 2) && (!strcmp(argv[1], DEV_DIAG_CLI_DEL_ERRCODE)))
        {
            dev_errcode_kv_del();
            aos_cli_printf("del err_code\r\n");
        }
    }
    else
    {
        aos_cli_printf("Error: %d\r\n", __LINE__);
        return;
    }
}
#endif

#ifdef MANUFACT_AP_FIND_ENABLE

#define MANUAP_CONTROL_KEY "manuap"
#define MANUAP_CLI_OPEN "open"
#define MANUAP_CLI_CLOSE "close"

static void handle_manuap_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    int ret = 0;
    uint8_t manuap_flag = 0;
    int manu_flag_len = sizeof(manuap_flag);
    if (argc == 1)
    {
        ret = aos_kv_get(MANUAP_CONTROL_KEY, &manuap_flag, &manu_flag_len);
        if (ret == 0)
        {
            if (manuap_flag)
            {
                aos_cli_printf("manuap state is open\r\n");
            }
            else
            {
                aos_cli_printf("manuap state is close\r\n");
            }
        }
        else
        {
            aos_cli_printf("no manuap state stored\r\n");
        }
    }
    else if (argc == 2)
    {
        if (!strcmp(argv[1], MANUAP_CLI_OPEN))
        {
            manuap_flag = 1;
            ret = aos_kv_set(MANUAP_CONTROL_KEY, &manuap_flag, sizeof(manuap_flag), 1);
            if (ret == 0)
            {
                aos_cli_printf("manuap opened\r\n");
            }
        }
        else if (!strcmp(argv[1], MANUAP_CLI_CLOSE))
        {
            manuap_flag = 0;
            ret = aos_kv_set(MANUAP_CONTROL_KEY, &manuap_flag, sizeof(manuap_flag), 1);
            if (ret == 0)
            {
                aos_cli_printf("manuap closed\r\n");
            }
        }
        else
        {
            aos_cli_printf("param input err\r\n");
        }
    }
    else
    {
        aos_cli_printf("param input err\r\n");
        return;
    }
}
#endif

static struct cli_command resetcmd = {.name = "reset",
                                      .help = "factory reset",
                                      .function = handle_reset_cmd};

static struct cli_command awss_enable_cmd = {.name = "active_awss",
                                             .help = "active_awss [start]",
                                             .function = handle_active_cmd};
static struct cli_command awss_dev_ap_cmd = {.name = "dev_ap",
                                             .help = "awss_dev_ap [start]",
                                             .function = handle_dev_ap_cmd};
static struct cli_command awss_cmd = {.name = "awss",
                                      .help = "awss [start]",
                                      .function = handle_awss_cmd};
static struct cli_command linkkeycmd = {.name = "linkkey",
                                        .help = "set/get linkkit keys. linkkey [devid] [<Product Key> <Device Name> <Device Secret> <Product Secret>]",
                                        .function = handle_linkkey_cmd};
static struct cli_command gw_ops = {.name = "gw",
                                    .help = "ops gw subdev. gw [add/del/reset/queryid subdevid] or gw [addall/update/delall/loginall/logoutall] or gw [addr/delr]",
                                    .function = handle_gw_cmd};
#ifdef DEV_ERRCODE_ENABLE
static struct cli_command dev_service_cmd = {.name = "dev_serv",
                                             .help = "device diagnosis service tests. like errcode, and log report",
                                             .function = handle_dev_serv_cmd};
#endif
#ifdef MANUFACT_AP_FIND_ENABLE
static struct cli_command manuap_cmd = {.name = "manuap",
                                        .help = "manuap [open] or manuap [close]",
                                        .function = handle_manuap_cmd};
#endif

int gateway_register_cmds(void)
{
    aos_cli_register_command(&resetcmd);
    aos_cli_register_command(&awss_enable_cmd);
    aos_cli_register_command(&awss_dev_ap_cmd);
    aos_cli_register_command(&awss_cmd);
    aos_cli_register_command(&linkkeycmd);
    aos_cli_register_command(&gw_ops);
#ifdef OFFLINE_LOG_UT_TEST
    aos_cli_register_command(&offline_log_cmd);
#endif
#ifdef DEV_ERRCODE_ENABLE
    aos_cli_register_command(&dev_service_cmd);
#endif
#ifdef MANUFACT_AP_FIND_ENABLE
    aos_cli_register_command(&manuap_cmd);
#endif

    return 0;
}
#endif
