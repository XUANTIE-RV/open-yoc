/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <aos/kernel.h>
#include <aos/yloop.h>

#include <yoc/atparser.h>
#include <drv/gpio.h>
#include <devices/netdrv.h>
#include <devices/impl/gprs_impl.h>

#include "ec200a_api.h"

static atparser_uservice_t *g_atparser_uservice_t;
static aos_mutex_t          g_cmd_mutex;

#define TAG "ec200a_api"

/*********************************************************************

ec200a api

**********************************************************************/
int ec200a_at0(void)
{
    int ret   = -1;
    int count = 3;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT") == 0) {
        while (count--) {
            if (atparser_recv(g_atparser_uservice_t, "OK") == 0) {
                ret = 0;
                break;
            }
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}

int ec200a_close_echo(void)
{
    int ret = -1;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "ATE0") == 0) {
        if (atparser_recv(g_atparser_uservice_t, "OK") == 0) {
            ret = 0;
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}

/********************************************************
    GPRS API

********************************************************/

int ec200a_check_simcard_is_insert(uint8_t isAsync)
{
    int ret = -1;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+CPIN?") == 0) {
        if ((atparser_recv(g_atparser_uservice_t, "+CPIN: READY") == 0)
            && (atparser_recv(g_atparser_uservice_t, "OK") == 0)) {
            ret = 0;
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}

int ec200a_check_signal_quality(rvm_hal_sig_qual_resp_t *respond, uint8_t isAsync)
{
    int ret = -1;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+CSQ") == 0) {
        if ((atparser_recv(g_atparser_uservice_t, "+CSQ:%d,%d", &respond->rssi, &respond->ber) == 0)
            && (atparser_recv(g_atparser_uservice_t, "OK") == 0))
        {
            ret = 0;
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}

int ec200a_check_register_status(rvm_hal_regs_stat_resp_t *respond, uint8_t isAsync)
{
    int ret = -1;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+CREG?") == 0) {
        if ((atparser_recv(g_atparser_uservice_t, "+CREG:%d,%d", &respond->n, &respond->stat) == 0)
            && (atparser_recv(g_atparser_uservice_t, "OK") == 0))
        {
            ret = 0;
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}

int ec200a_check_gprs_service_status(int *sstat, uint8_t isAsync)
{
    int ret = -1;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+CGATT?") == 0) {
        if ((atparser_recv(g_atparser_uservice_t, "+CGATT:%d", sstat) == 0)
            && (atparser_recv(g_atparser_uservice_t, "OK") == 0))
        {
            ret = 0;
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}

int ec200a_set_gprs_service_status(int state, uint8_t isAsync)
{
    int ret = -1;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+CGATT=%d", state) == 0) {
        if (atparser_recv(g_atparser_uservice_t, "OK") == 0) {
            ret = 0;
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}

int ec200a_connect_to_network(uint8_t isAsync)
{
    int ret = -1;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+QNETDEVCTL=1,1,1") == 0) {
        if (atparser_recv(g_atparser_uservice_t, "OK") == 0) {
            ret = 0;
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}

int ec200a_get_ccid(char ccid[21])
{
    int ret = -1;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+CCID") == 0) {
        if ((atparser_recv(g_atparser_uservice_t, "+CCID:%s", ccid) == 0)
            && (atparser_recv(g_atparser_uservice_t, "OK") == 0)) {
            ret = 0;
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}

int ec200a_check_net_type(int *net_type)
{
    int ret = -1;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+QCFG=\"usbnet\"") == 0) {
        if ((atparser_recv(g_atparser_uservice_t, "+QCFG: \"usbnet\",%d", net_type) == 0)
            && (atparser_recv(g_atparser_uservice_t, "OK") == 0))
        {
            ret = 0;
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}

int ec200a_set_net_type(int net_type)
{
    int ret = -1;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+QCFG=\"usbnet\",%d", net_type) == 0) {
        if ((atparser_recv(g_atparser_uservice_t, "OK") == 0)) {
            ret = 0;
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}

void ec200a_set_timeout(int ms)
{
    atparser_set_timeout(g_atparser_uservice_t, ms);
}

static int ec200a_module_inited = 0;
int        ec200a_module_init(char *deveice_name)
{
    rvm_hal_uart_config_t config;

    if (ec200a_module_inited) {
        return 0;
    }

    rvm_hal_uart_config_default(&config);

    utask_t *task = utask_new("ec200a", 2 * 1024, QUEUE_MSG_COUNT, AOS_DEFAULT_APP_PRI + 4);

    if (task == NULL) {
        return -1;
    }

    rvm_hal_uart_config_default(&config);

    g_atparser_uservice_t = atparser_init(task, deveice_name, &config);

    if (g_atparser_uservice_t == NULL) {
        LOGI(TAG, "atparser_init FAIL");
        return -1;
    }

    aos_mutex_new(&g_cmd_mutex);

    atparser_debug_control(g_atparser_uservice_t, 1);

    ec200a_module_inited = 1;
    return 0;
}
