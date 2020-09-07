/*
 * Copyright (C) 2020 FishSemi Inc. All rights reserved.
 */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <aos/kernel.h>
#include <aos/log.h>
#include <aos/yloop.h>

#include <yoc/atparser.h>
#include <devices/netdrv.h>
#include <devices/hal/nbiot_impl.h>
#include <devices/u1.h>

#include "u1_api.h"

static atparser_uservice_t *g_atparser_uservice_t;
static int g_nbiot_start_flg = 0;
static int g_nbiot_reg_status = NBIOT_STATUS_NOREG;
static aos_mutex_t g_cmd_mutex;
static aos_mutex_t g_status_mutex;
static aos_timer_t g_status_timer;

#define STATUS_TIMEOUT 30000

#define TAG "u1_api"

static void u1_change_nbiot_status(int new_status)
{
    int old_status;

    aos_mutex_lock(&g_status_mutex, AOS_WAIT_FOREVER);
    old_status = g_nbiot_reg_status;
    g_nbiot_reg_status = new_status;
    aos_mutex_unlock(&g_status_mutex);

    if (old_status != NBIOT_STATUS_REGISTERED && \
        new_status == NBIOT_STATUS_REGISTERED) {
        aos_timer_stop(&g_status_timer);
        event_publish(EVENT_NBIOT_LINK_UP, NULL);
    } else if (old_status == NBIOT_STATUS_REGISTERED && \
        new_status != NBIOT_STATUS_REGISTERED) {
        event_publish(EVENT_NBIOT_LINK_DOWN, NULL);
    }
}

static void status_timer_entry(void *timer, void *arg)
{

    aos_mutex_lock(&g_status_mutex, AOS_WAIT_FOREVER);
    g_nbiot_reg_status = NBIOT_STATUS_NOREG;
    aos_mutex_unlock(&g_status_mutex);

    event_publish(EVENT_NBIOT_LINK_DOWN, NULL);
}

static int u1_register_status_handler(atparser_uservice_t *at, void *priv, oob_data_t *oob_data)
{
    int reg_stat;
    int len;
    char *str = strchr(oob_data->buf, '\n');

    if (str != NULL) {
        len = sscanf(oob_data->buf, "%d", &reg_stat);
        oob_data->used_len = len;
        u1_change_nbiot_status(reg_stat);
    } else {
        return -1;
    }

    return 0;
}

static int u1_sim_status_handler(atparser_uservice_t *at, void *priv, oob_data_t *oob_data)
{
    int sim_stat;
    int len;
    char *str = strchr(oob_data->buf, '\n');

    if (str != NULL) {
        len = sscanf(oob_data->buf, "%d", &sim_stat);
        oob_data->used_len = len;
        LOGI(TAG, "sim status:%d", sim_stat);
    } else {
        return -1;
    }

    return 0;
}

static int u1_signal_indicate_handler(atparser_uservice_t *at, void *priv, oob_data_t *oob_data)
{
    int rsrp, rsrq;
    int len;
    char *str = strchr(oob_data->buf, '\n');

    if (str != NULL) {
        len = sscanf(oob_data->buf, "%*d,%*d,%*d,%*d,%d,%d", &rsrq, &rsrp);
        oob_data->used_len = len;
        LOGI(TAG, "singal indicate:rsrp %d, rsrq %d", rsrp, rsrq);
    } else {
        return -1;
    }

    return 0;
}

/*********************************************************************

u1 api

**********************************************************************/
int u1_at0(void)
{
    int ret = -1;
    int count = 3;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT") == 0) {
        while (count --) {
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

int u1_get_imei(nbiot_imei_t *pimei)
{
    int ret = -1;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+CGSN") == 0) {
        if ((atparser_recv(g_atparser_uservice_t, "+CGSN: %s", pimei->imei) == 0)\
            && (atparser_recv(g_atparser_uservice_t, "OK") == 0)) {
            ret = 0;
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}

int u1_get_imsi(nbiot_imsi_t *pimsi)
{
    int ret = -1;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+CIMI") == 0) {
        if ((atparser_recv(g_atparser_uservice_t, "%s", pimsi->imsi) == 0)\
            && (atparser_recv(g_atparser_uservice_t, "OK") == 0)) {
            ret = 0;
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}

int u1_get_iccid(nbiot_iccid_t *piccid)
{
    int ret = -1;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+NCCID") == 0) {
        if ((atparser_recv(g_atparser_uservice_t, "+NCCID: %s", piccid->iccid) == 0)\
            && (atparser_recv(g_atparser_uservice_t, "OK") == 0)) {
            ret = 0;
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}

int u1_start_nbiot(void)
{
    int ret = -1;
    int count = 2;

    if (g_nbiot_start_flg == 1) {
        return 0;
    }

    aos_timer_start(&g_status_timer);

    u1_set_register_status_ind(1);

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+CFUN=1") == 0) {
        while (count--) {
            if (atparser_recv(g_atparser_uservice_t, "OK") == 0) {
                g_nbiot_start_flg = 1;
                ret = 0;
                break;
            }
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}

int u1_stop_nbiot(void)
{
    int ret = -1;

    if (g_nbiot_start_flg == 0) {
        return 0;
    }

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+CFUN=0") == 0) {
        if ((atparser_recv(g_atparser_uservice_t, "OK") == 0)) {
            g_nbiot_start_flg = 0;
            ret = 0;
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    aos_timer_stop(&g_status_timer);

    return ret;
}

int u1_reset_nbiot(void)
{
    u1_stop_nbiot();
    sleep(1);
    u1_start_nbiot();

    return 0;
}

int u1_set_register_status_ind(int status)
{
    int ret = -1;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+CEREG=%d", status) == 0) {
        if (atparser_recv(g_atparser_uservice_t, "OK") == 0) {
            ret = 0;
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}

int u1_check_register_status(nbiot_regs_stat_resp_t *respond)
{
    int ret = -1;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+CEREG?") == 0) {
        if ((atparser_recv(g_atparser_uservice_t, "+CEREG: %d,%d", &respond->n, &respond->stat) == 0) \
            && (atparser_recv(g_atparser_uservice_t, "OK") == 0)) {
            u1_change_nbiot_status(respond->stat);
            ret = 0;
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}

int u1_set_signal_strength_ind(int status)
{
    int ret = -1;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT*CSQ=%d", status) == 0) {
        if (atparser_recv(g_atparser_uservice_t, "OK") == 0) {
            ret = 0;
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}

int u1_check_signal_quality(nbiot_sig_qual_resp_t *respond)
{
    int ret = -1;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+CSQ") == 0) {
        if ((atparser_recv(g_atparser_uservice_t, "+CSQ: %d,%d", &respond->rssi, &respond->ber) == 0) \
            && (atparser_recv(g_atparser_uservice_t, "OK") == 0)) {
            ret = 0;
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}

int u1_check_simcard_is_insert(int *pstatus)
{
    int ret = -1;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT^SIMST?") == 0) {
        if ((atparser_recv(g_atparser_uservice_t, "^SIMST: %d", pstatus) == 0) \
            && (atparser_recv(g_atparser_uservice_t, "OK") == 0)) {
            ret = 0;
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}

int u1_get_local_ip(char ip[16])
{
    int ret = -1;
    int cid;
    int reg_status;

    aos_mutex_lock(&g_status_mutex, AOS_WAIT_FOREVER);
    reg_status = g_nbiot_reg_status;
    aos_mutex_unlock(&g_status_mutex);

    if (reg_status != NBIOT_STATUS_REGISTERED) {
        strcpy(ip, "0.0.0.0");
        return -1;
    }

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+CGDCONT?") == 0) {
        if ((atparser_recv(g_atparser_uservice_t, "+CGDCONT: %d,\"%*[^\"]\",\"\",\"%[^\"]%*s", &cid, ip) == 0) \
            && (atparser_recv(g_atparser_uservice_t, "OK") == 0)) {
            ret = 0;
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}

int u1_get_cell_info(nbiot_cell_info_t *pcellinfo)
{
    int ret = -1;

    aos_mutex_lock(&g_cmd_mutex, AOS_WAIT_FOREVER);

    atparser_clr_buf(g_atparser_uservice_t);

    if (atparser_send(g_atparser_uservice_t, "AT+NCCELLINFO") == 0) {
        if ((atparser_recv(g_atparser_uservice_t, "+NCCELLINFO:") == 0)
            && (atparser_recv(g_atparser_uservice_t, "MCC:%s", pcellinfo->mcc) == 0)
            && (atparser_recv(g_atparser_uservice_t, "MNC:%s", pcellinfo->mnc) == 0)
            && (atparser_recv(g_atparser_uservice_t, "CURR_MODE:%*d") == 0)
            && (atparser_recv(g_atparser_uservice_t, "DUPLEX MODE:%*s") == 0)
            && (atparser_recv(g_atparser_uservice_t, "UE_CATEORY:%d", &pcellinfo->ue_category) == 0)
            && (atparser_recv(g_atparser_uservice_t, "CELL_ID:%x", &pcellinfo->cellId) == 0)
            && (atparser_recv(g_atparser_uservice_t, "LAC_ID:%x", &pcellinfo->lacId) == 0)
            && (atparser_recv(g_atparser_uservice_t, "RSRP:%d", &pcellinfo->rsrp) == 0)
            && (atparser_recv(g_atparser_uservice_t, "RSRQ:%d", &pcellinfo->rsrq) == 0)
            && (atparser_recv(g_atparser_uservice_t, "SNR:%d", &pcellinfo->snr) == 0)
            && (atparser_recv(g_atparser_uservice_t, "BAND:%d", &pcellinfo->band) == 0)
            && (atparser_recv(g_atparser_uservice_t, "ARFCN:%d", &pcellinfo->arfcn) == 0)
            && (atparser_recv(g_atparser_uservice_t, "PCI:%d", &pcellinfo->pci) == 0)
            && (atparser_recv(g_atparser_uservice_t, "DL MCS:%*d") == 0)
            && (atparser_recv(g_atparser_uservice_t, "UL MCS:%*d") == 0)
            && (atparser_recv(g_atparser_uservice_t, "OK") == 0)) {
            ret = 0;
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t);

    aos_mutex_unlock(&g_cmd_mutex);

    return ret;
}

int u1_module_init(utask_t *task, u1_nbiot_param_t *param)
{
    int ret;

    if (task == NULL) {
        task = utask_new("u1_modem", 2 * 1024, QUEUE_MSG_COUNT, AOS_DEFAULT_APP_PRI + 4);
    }

    if (task == NULL) {
        goto err_out;
    }

    g_atparser_uservice_t = atparser_init(task, param->device_name, NULL);

    if (g_atparser_uservice_t == NULL) {
        LOGE(TAG, "atparser_init FAIL");
        goto with_utask_err;
    }

    ret = aos_mutex_new(&g_cmd_mutex);
    if (ret < 0) {
        LOGE(TAG, "atparser_init mutex FAIL");
        goto with_utask_err;
    }

    ret = aos_mutex_new(&g_status_mutex);
    if (ret < 0) {
        LOGE(TAG, "atparser_init mutex FAIL");
        goto with_cmd_mutex_err;
    }

    atparser_debug_control(g_atparser_uservice_t, 1);
    atparser_oob_create(g_atparser_uservice_t, "+CEREG: ", u1_register_status_handler, NULL);
    atparser_oob_create(g_atparser_uservice_t, "+CESQ: ", u1_signal_indicate_handler, NULL);
    sleep(3);
    aos_timer_new_ext(&g_status_timer, status_timer_entry, NULL, STATUS_TIMEOUT, 0, 0);

    return 0;

with_cmd_mutex_err:
    aos_mutex_free(&g_cmd_mutex);

with_utask_err:
    utask_destroy(task);

err_out:
    return -1;
}

