/*
 * Copyright (C) 2015-2022 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <aos/kernel.h>
#include "cJSON.h"
#include "smartliving/iot_import.h"
#include "smartliving/exports/iot_export_linkkit.h"
#include "aos/kv.h"
#include "app_main.h"
#include "app_gateway_main.h"
#include "app_gateway_cmds.h"
#include "app_gateway_api.h"
#include "app_gateway_ut.h"
#include <key_mgr.h>
#include "provisioner_main.h"
#include "common/log.h"
#include "gateway_occ_auth.h"
#include "gateway.h"

#if defined(CONFIG_SMARTLIVING_DEMO) && CONFIG_SMARTLIVING_DEMO

#define TAG "gateway_ut"

static aos_queue_t          gateway_queue_t;
static char                 gateway_queue_buf[MAX_QUEUE_SIZE];
static int                  bind_missed_devid;
static gw_topo_get_reason_e topo_get_reason             = GW_TOPO_GET_REASON_MAX;
gw_subdev_cloud_info_t *    g_gateway_subdev_cloud_info = NULL;
static gw_subdev_t          _gateway_ut_get_subdev(int devid);
static void                 _gateway_ut_arrange_cloud_id(void);
static void                 _gateway_subdev_delete_handle(int devid);

static int _gateway_subdev_cloud_info_store(int16_t idx, subdev_cloud_info_t *info)
{
    char key_string[12] = { 0 };
    int  ret            = 0;

    sprintf(key_string, "subcld-%04x", idx);

    ret = aos_kv_set(key_string, info, sizeof(subdev_cloud_info_t), 1);
    if (ret != 0) {
        LOGE(TAG, "save subcld[%d] info fail", idx);
    }

    return ret;
}

static int _gateway_subdev_cloud_info_get(int idx, subdev_cloud_info_t *info)
{
    char key_string[12] = { 0 };
    int  buffer_len     = sizeof(subdev_cloud_info_t);

    sprintf(key_string, "subcld-%04x", idx);

    return aos_kv_get(key_string, info, &buffer_len);
}

static int _gateway_subdev_cloud_info_delete(int16_t idx)
{
    char key_string[12] = { 0 };
    int  ret            = 0;

    sprintf(key_string, "subcld-%04x", idx);

    ret = aos_kv_del(key_string);
    if (ret != 0) {
        LOGE(TAG, "del subcld[%d] info fail", idx);
    }

    return ret;
}

int app_gateway_ut_send_msg(gateway_msg_t *msg, int len)
{
    if (msg && len == QUEUE_MSG_SIZE) {
        gateway_info("send msg type(%d)", msg->msg_type);
        return aos_queue_send(&gateway_queue_t, msg, len);
    }

    gateway_err("param err");

    return -1;
}

static uint8_t _gw_subdev_del(gw_subdev_t subdev, void *data)
{
    gateway_subdev_del(subdev);
    
    return GW_SUBDEV_ITER_CONTINUE;
}

int app_gateway_ut_reset()
{
    for (int i = 1; i < MAX_DEVICES_META_NUM + 1; i++) {
        if (g_gateway_subdev_cloud_info->info[i].cloud_devid != GATEWAY_NODE_INVALID) {
            iotx_linkkit_dev_meta_info_t *p_subdev = &(g_gateway_subdev_cloud_info->info[i].meta);
            gateway_reset_subdev(p_subdev);
            g_gateway_subdev_cloud_info->info[i].cloud_devid = GATEWAY_NODE_INVALID;
            g_gateway_subdev_cloud_info->info[i].bind        = GATEWAY_NODE_UNBIND;

            _gateway_subdev_cloud_info_delete(i);
        }
    }
    aos_kv_del("gw_magic_code");

    aos_kv_del("hal_devinfo_pk");
    aos_kv_del("hal_devinfo_ps");
    aos_kv_del("hal_devinfo_dn");
    aos_kv_del("hal_devinfo_ds");
    aos_kv_del("hal_devinfo_pid");

    gateway_subdev_foreach(_gw_subdev_del, NULL);
    return 0;
}

static int _gateway_connect_cloud(char *payload, int payload_len)
{
    int            index        = 0;
    int            subdev_total = 0;
    int            subdev_num   = 0;
    cJSON *        topo_list = NULL, *subdev = NULL;
    cJSON *        pk = NULL, *dn = NULL;
    gateway_ctx_t *p_gateway_ctx = gateway_get_ctx();

    iotx_linkkit_dev_meta_info_t *p_subdev_mate       = NULL;
    iotx_linkkit_dev_meta_info_t *p_subdev_mate_index = NULL;

    if (payload == NULL || payload_len < 1) {
        gateway_err("param err");
        return -1;
    }
    /* Parse Request */
    topo_list = cJSON_Parse(payload);
    if (topo_list == NULL || !cJSON_IsArray(topo_list)) {
        gateway_err("topo list json format err");
        return -1;
    }

    subdev_total = cJSON_GetArraySize(topo_list);
    if (subdev_total < 1) {
        gateway_err("topo is empty");
        return -1;
    }

    p_subdev_mate = HAL_Malloc(sizeof(iotx_linkkit_dev_meta_info_t) * subdev_total);
    if (p_subdev_mate == NULL) {
        gateway_err("no mem");
        return -1;
    }

    memset(p_subdev_mate, 0, sizeof(iotx_linkkit_dev_meta_info_t) * subdev_total);
    for (index = 0; index < subdev_total; index++) {
        subdev = cJSON_GetArrayItem(topo_list, index);
        if (subdev == NULL || !cJSON_IsObject(subdev)) {
            gateway_err("subdev json err");
            continue;
        }

        pk = cJSON_GetObjectItem(subdev, TOPO_LIST_PK);
        dn = cJSON_GetObjectItem(subdev, TOPO_LIST_DN);
        if (cJSON_IsString(pk) && cJSON_IsString(dn)) {
            p_subdev_mate_index = p_subdev_mate + subdev_num;
            subdev_num++;
            HAL_Snprintf(p_subdev_mate_index->product_key, PRODUCT_KEY_MAXLEN, "%s", pk->valuestring);
            HAL_Snprintf(p_subdev_mate_index->device_name, DEVICE_NAME_MAXLEN, "%s", dn->valuestring);
        }
    }

    cJSON_Delete(topo_list);

    gateway_add_multi_subdev(p_gateway_ctx->master_devid, p_subdev_mate, subdev_num);
    if (p_subdev_mate)
        HAL_Free(p_subdev_mate);

    return 0;
}

int app_gateway_ut_handle_topolist_reply(const char *payload, const int payload_len)
{
    gateway_msg_t msg;

    if (topo_get_reason != GW_TOPO_GET_REASON_CONNECT_CLOUD) {
        return 0;
    }

    memset(&msg, 0, sizeof(gateway_msg_t));
    msg.payload = HAL_Malloc(payload_len + 1);

    if (msg.payload) {
        memset(msg.payload, '\0', payload_len + 1);
        memcpy(msg.payload, payload, payload_len);
        msg.msg_type    = GATEWAY_MSG_TYPE_CLOUD_CONNECT;
        msg.devid       = 1;
        msg.payload_len = payload_len;
        app_gateway_ut_send_msg(&msg, sizeof(gateway_msg_t));
    }

    return 0;
}

int app_gateway_ut_handle_permit_join(void)
{
    gateway_msg_t msg;

    msg.msg_type = GATEWAY_MSG_TYPE_PERMIT_JOIN;
    msg.devid    = 1;
    app_gateway_ut_send_msg(&msg, sizeof(gateway_msg_t));

    return 0;
}

int app_gateway_ut_handle_property_set(const int devid, const char *request, const int request_len)
{
    int         res    = 0;
    gw_subdev_t subdev = 0;
    gateway_debug("Property Set Received, Devid: %d, Request: %s", devid, request);

    subdev = _gateway_ut_get_subdev(devid);
    if (subdev < 0) {
        gateway_err("invalid subdev, devid is %d", devid);
        return -1;
    }

    gateway_subdev_model_ctrl(subdev, (void *)request, request_len);

    cJSON *root = NULL;

    /* Parse Root */
    root = cJSON_Parse(request);

    if (root == NULL || !cJSON_IsObject(root)) {
        printf("JSON Parse Error\n");
        return -1;
    }

    cJSON *item = cJSON_GetObjectItem(root, "powerstate");

    if (item && cJSON_IsNumber(item)) {
        uint8_t led_onoff = 0;

        led_onoff = (item->valueint != 0) ? 1 : 0;
        gateway_subdev_set_onoff(subdev, led_onoff);
    }

    item = cJSON_GetObjectItem(root, "brightness");

    if (item && cJSON_IsNumber(item)) {
        gateway_info("turn bri %d", item->valueint);
        gateway_subdev_set_brightness(subdev, item->valueint);
    }

    /* add here */

    cJSON_Delete(root);

    res = IOT_Linkkit_Report(devid, ITM_MSG_POST_PROPERTY, (unsigned char *)request, request_len);

    gateway_info("Post Property Message ID: %d, payload %s", res, request);

    return 0;
}

static int _get_one_device_mate(int devid, iotx_linkkit_dev_meta_info_t *p_subdev)
{
    if (!p_subdev)
        return -1;

    if (g_gateway_subdev_cloud_info->info[devid].bind == GATEWAY_NODE_BINDED)
        return -1;

    if (g_gateway_subdev_cloud_info->info[devid].cloud_devid == GATEWAY_NODE_INVALID)
        return -1;

    strcpy(p_subdev->device_name, g_gateway_subdev_cloud_info->info[devid].meta.device_name);
    strcpy(p_subdev->device_secret, g_gateway_subdev_cloud_info->info[devid].meta.device_secret);
    strcpy(p_subdev->product_key, g_gateway_subdev_cloud_info->info[devid].meta.product_key);
    strcpy(p_subdev->product_secret, g_gateway_subdev_cloud_info->info[devid].meta.product_secret);

    gateway_info("DevNum(%d) DN:%s", devid, p_subdev->device_name);

    return 0;
}

// get multi sub devices mate info from KV,just for testing
static int _get_all_subdev_mate(iotx_linkkit_dev_meta_info_t *subdev_meta)
{
    int index = 0;

    iotx_linkkit_dev_meta_info_t *p_subdev = NULL;

    if (!subdev_meta)
        return -1;

    for (index = 1; index < MAX_DEVICES_META_NUM; index++) {
        p_subdev = subdev_meta + (index - 1);

        _get_one_device_mate(index, p_subdev);
    }

    return 0;
}

// get multi sub devices mate info from KV,just for testing
static int _get_range_subdev_meta(iotx_linkkit_dev_meta_info_t *subdev_meta, int devid_start, int devid_end)
{
    int index = 0;

    iotx_linkkit_dev_meta_info_t *p_subdev = NULL;

    if (!subdev_meta)
        return -1;

    if (devid_start > 0 && devid_start < MAX_DEVICES_META_NUM && devid_end > 0 && devid_end < MAX_DEVICES_META_NUM
        && devid_start < devid_end)
    {
        for (index = devid_start; index <= devid_end; index++) {
            p_subdev = subdev_meta + (index - 1);

            _get_one_device_mate(index, p_subdev);
        }
    } else {
        gateway_warn("devid err");
    }

    return 0;
}

int app_gateway_ut_update_subdev(gw_topo_get_reason_e reason)
{
    gateway_ctx_t *gateway_ctx = gateway_get_ctx();
    gateway_info("do update subdev");

    IOT_Linkkit_Query(gateway_ctx->master_devid, ITM_MSG_QUERY_TOPOLIST, NULL, 0);
    topo_get_reason = reason;

    return 0;
}

int app_gateway_ut_msg_process(int master_devid, int timeout)
{
    int            ret       = 0;
    unsigned int   recv_size = 0;
    int            subdev_id = -1;
    gateway_msg_t  msg;
    gateway_ctx_t *gateway_ctx = gateway_get_ctx();

    ret = aos_queue_recv(&gateway_queue_t, timeout, &msg, (size_t *)&recv_size);
    if (ret == 0 && recv_size == QUEUE_MSG_SIZE) {
        gateway_info("msg.type:%d, devid:%d", msg.msg_type, msg.devid);
        if (msg.devid < 1 || msg.devid > MAX_DEVICES_META_NUM - 1) {
            gateway_warn("sub dev id(%d) err", msg.devid);
            return -1;
        }

        switch (msg.msg_type) {
            case GATEWAY_MSG_TYPE_ADD: {
                iotx_linkkit_dev_meta_info_t subdev_mate;
                memset(&subdev_mate, 0, sizeof(subdev_mate));
                _get_one_device_mate(msg.devid, &subdev_mate);

                if (gateway_ctx->permit_join == 1 && strlen(gateway_ctx->permit_join_pk) > 0) {
                    if (strcmp(subdev_mate.product_key, gateway_ctx->permit_join_pk)) {
                        gateway_warn("permit join pk:%s is not found", gateway_ctx->permit_join_pk);
                        ret = -1;
                        break;
                    }
                }

                ret = gateway_add_subdev(&subdev_mate);
            } break;
            case GATEWAY_MSG_TYPE_ADDALL: {
                iotx_linkkit_dev_meta_info_t *p_meta_info = NULL;

                p_meta_info = HAL_Malloc(sizeof(iotx_linkkit_dev_meta_info_t) * (MAX_DEVICES_META_NUM - 1));
                if (!p_meta_info) {
                    gateway_err("no mem");
                    return -1;
                }

                memset(p_meta_info, '\0', sizeof(iotx_linkkit_dev_meta_info_t) * (MAX_DEVICES_META_NUM - 1));
                _get_all_subdev_mate(p_meta_info);
                ret = gateway_add_multi_subdev(master_devid, p_meta_info, MAX_DEVICES_META_NUM - 1);

                HAL_Free(p_meta_info);
            } break;
            case GATEWAY_MSG_TYPE_ADD_RANGE: {
                iotx_linkkit_dev_meta_info_t *p_meta_info = NULL;
                int                           subdev_num  = msg.devid_end - msg.devid + 1;

                if (msg.devid < 1 || msg.devid >= MAX_DEVICES_META_NUM || msg.devid_end < 1
                    || msg.devid_end >= MAX_DEVICES_META_NUM || msg.devid >= msg.devid_end)
                {
                    gateway_warn("sub dev id err");
                    return -1;
                }

                p_meta_info = HAL_Malloc(sizeof(iotx_linkkit_dev_meta_info_t) * subdev_num);
                if (!p_meta_info) {
                    gateway_err("no mem");
                    return -1;
                }

                memset(p_meta_info, '\0', sizeof(iotx_linkkit_dev_meta_info_t) * subdev_num);
                _get_range_subdev_meta(p_meta_info, msg.devid, msg.devid_end);
                ret = gateway_add_multi_subdev(master_devid, p_meta_info, subdev_num);

                HAL_Free(p_meta_info);
            } break;
            case GATEWAY_MSG_TYPE_DELALL: {
                iotx_linkkit_dev_meta_info_t subdev_mate;
                for (subdev_id = 1; subdev_id < MAX_DEVICES_META_NUM; subdev_id++) {

                    memset(&subdev_mate, 0, sizeof(subdev_mate));
                    _get_one_device_mate(subdev_id, &subdev_mate);
                    ret = gateway_del_subdev(&subdev_mate);
                    _gateway_subdev_delete_handle(msg.devid);
                    gateway_info("del subdev id(%d) ret = %d", subdev_id, ret);
                }
            } break;
            case GATEWAY_MSG_TYPE_DEL: {
                iotx_linkkit_dev_meta_info_t subdev_mate;

                memset(&subdev_mate, 0, sizeof(subdev_mate));
                _get_one_device_mate(msg.devid, &subdev_mate);
                ret = gateway_del_subdev(&subdev_mate);

                _gateway_subdev_delete_handle(msg.devid);
            } break;

            case GATEWAY_MSG_TYPE_DEL_RANGE: {
                iotx_linkkit_dev_meta_info_t subdev_mate;

                if (msg.devid < 1 || msg.devid >= MAX_DEVICES_META_NUM || msg.devid_end < 1
                    || msg.devid_end >= MAX_DEVICES_META_NUM || msg.devid >= msg.devid_end)
                {
                    gateway_warn("sub dev id err");
                    return -1;
                }

                for (subdev_id = msg.devid; subdev_id <= msg.devid_end; subdev_id++) {

                    memset(&subdev_mate, 0, sizeof(subdev_mate));
                    _get_one_device_mate(subdev_id, &subdev_mate);
                    ret = gateway_del_subdev(&subdev_mate);
                    gateway_info("del subdev id(%d) ret = %d", subdev_id, ret);
                }
            } break;
            case GATEWAY_MSG_TYPE_UPDATE: {
                ret = app_gateway_ut_update_subdev(GW_TOPO_GET_REASON_CLI_CMD);
            } break;
            case GATEWAY_MSG_TYPE_LOGIN_ALL: {
                iotx_linkkit_dev_meta_info_t *p_meta_info = NULL;

                p_meta_info = HAL_Malloc(sizeof(iotx_linkkit_dev_meta_info_t) * (MAX_DEVICES_META_NUM - 1));
                if (!p_meta_info) {
                    gateway_err("no mem");
                    return -1;
                }

                memset(p_meta_info, '\0', sizeof(iotx_linkkit_dev_meta_info_t) * (MAX_DEVICES_META_NUM - 1));
                _get_all_subdev_mate(p_meta_info);
                ret = gateway_batch_login(master_devid, p_meta_info, MAX_DEVICES_META_NUM - 1);

                HAL_Free(p_meta_info);
            } break;
            case GATEWAY_MSG_TYPE_LOGOUT_ALL: {
                iotx_linkkit_dev_meta_info_t *p_meta_info = NULL;

                p_meta_info = HAL_Malloc(sizeof(iotx_linkkit_dev_meta_info_t) * (MAX_DEVICES_META_NUM - 1));
                if (!p_meta_info) {
                    gateway_err("no mem");
                    return -1;
                }

                memset(p_meta_info, '\0', sizeof(iotx_linkkit_dev_meta_info_t) * (MAX_DEVICES_META_NUM - 1));
                _get_all_subdev_mate(p_meta_info);
                ret = gateway_batch_logout(master_devid, p_meta_info, MAX_DEVICES_META_NUM - 1);

                HAL_Free(p_meta_info);
            } break;
            case GATEWAY_MSG_TYPE_RESET: {
                iotx_linkkit_dev_meta_info_t subdev_mate;

                memset(&subdev_mate, 0, sizeof(subdev_mate));
                _get_one_device_mate(msg.devid, &subdev_mate);
                ret = gateway_reset_subdev(&subdev_mate);
            } break;
            case GATEWAY_MSG_TYPE_QUERY_SUBDEV_ID: {
                iotx_linkkit_dev_meta_info_t subdev_mate;

                memset(&subdev_mate, 0, sizeof(subdev_mate));
                _get_one_device_mate(msg.devid, &subdev_mate);
                ret = gateway_query_subdev_id(gateway_ctx->master_devid, &subdev_mate);
            } break;
            case GATEWAY_MSG_TYPE_CLOUD_CONNECT: {
                if (msg.payload) {
                    _gateway_connect_cloud(msg.payload, msg.payload_len);
                    HAL_Free(msg.payload);
                }
            } break;
            case GATEWAY_MSG_TYPE_PERMIT_JOIN: {
                if (gateway_ctx->permit_join == 1) {
                    int                           matched_subdev_num = 0;
                    iotx_linkkit_dev_meta_info_t  subdev_mate        = { 0 };
                    iotx_linkkit_dev_meta_info_t *p_subdev_mate      = NULL;

                    bind_missed_devid = 0;
                    p_subdev_mate     = HAL_Malloc(sizeof(iotx_linkkit_dev_meta_info_t) * MAX_DEVICES_META_NUM);
                    if (p_subdev_mate == NULL) {
                        gateway_err("no mem");
                        return -1;
                    }

                    memset(p_subdev_mate, 0, sizeof(iotx_linkkit_dev_meta_info_t) * MAX_DEVICES_META_NUM);
                    for (subdev_id = 1; subdev_id < MAX_DEVICES_META_NUM; subdev_id++) {
                        memset(&subdev_mate, 0, sizeof(subdev_mate));
                        if (0 == _get_one_device_mate(subdev_id, &subdev_mate)
                            && 0 == strcmp(gateway_ctx->permit_join_pk, subdev_mate.product_key))
                        {
                            memcpy(p_subdev_mate + matched_subdev_num, &subdev_mate,
                                   sizeof(iotx_linkkit_dev_meta_info_t));
                            matched_subdev_num++;
                        } else {
                            // gateway_warn("permit join pk:%s found pk:%s dn:%s", gateway_ctx->permit_join_pk,
                            // subdev_mate.product_key, subdev_mate.device_name);
                        }
                    }

                    gateway_add_multi_subdev(gateway_ctx->master_devid, p_subdev_mate, matched_subdev_num);
                    _gateway_ut_arrange_cloud_id();
                    if (bind_missed_devid > 0) {
                        app_gateway_subdev_binded_handle(bind_missed_devid);
                    }

                    if (p_subdev_mate) {
                        HAL_Free(p_subdev_mate);
                    }
                }
            } break;
            default:
                gateway_warn("unKnow msg type(%d)", msg.msg_type);
                return -1;
        }
    }

    return ret;
}

static void _gateway_ut_arrange_cloud_id(void)
{
    short          subdev_id      = 0;
    gateway_ctx_t *gateway_ctx    = gateway_get_ctx();

    for (int i = 1; i < MAX_DEVICES_META_NUM + 1; i++) {
        if (g_gateway_subdev_cloud_info->info[i].cloud_devid != GATEWAY_NODE_INVALID) {
            subdev_id
                = gateway_query_subdev_id(gateway_ctx->master_devid, &(g_gateway_subdev_cloud_info->info[i].meta));
            if ((subdev_id > 0) && (g_gateway_subdev_cloud_info->info[i].cloud_devid != subdev_id)) {
                gateway_info("dev[%d] cloud_id[%d --> %d]", g_gateway_subdev_cloud_info->info[i].cloud_devid,
                             subdev_id);
                g_gateway_subdev_cloud_info->info[i].cloud_devid = subdev_id;

                _gateway_subdev_cloud_info_store(i, &(g_gateway_subdev_cloud_info->info[i]));
            }
            continue;
        }
    }
}

void app_gateway_subdev_registered_handle(int devid)
{
    if (devid == 0) { /* master id 0 is for gateway itself */
        g_gateway_subdev_cloud_info->info[0].cloud_devid = devid;
        return;
    }

    /* check if the cloud_id has been duplicated */
    for (int i = 1; i < MAX_DEVICES_META_NUM + 1; i++) {
        if (g_gateway_subdev_cloud_info->info[i].cloud_devid > GATEWAY_NODE_INVALID) {
            if (g_gateway_subdev_cloud_info->info[i].cloud_devid == devid) {
                return;
            }
        }
    }

    for (int i = 1; i < MAX_DEVICES_META_NUM + 1; i++) {
        if (g_gateway_subdev_cloud_info->info[i].cloud_devid == GATEWAY_NODE_UNREG) {
            g_gateway_subdev_cloud_info->info[i].cloud_devid = devid;
            _gateway_subdev_cloud_info_store(i, &(g_gateway_subdev_cloud_info->info[i]));
            break;
        }
    }
}

void app_gateway_subdev_binded_handle(int devid)
{
    int  i;

    if (devid == 0) { // master id 0 is for gateway itself
        g_gateway_subdev_cloud_info->info[0].cloud_devid = devid;
        _gateway_subdev_cloud_info_store(0, &(g_gateway_subdev_cloud_info->info[0]));
        return;
    }

    for (i = 1; i < MAX_DEVICES_META_NUM + 1; i++) {
        if (g_gateway_subdev_cloud_info->info[i].cloud_devid == devid) {
            gateway_info("Bind dev[%d]", i);
            if (g_gateway_subdev_cloud_info->info[i].bind == GATEWAY_NODE_UNBIND) {
                g_gateway_subdev_cloud_info->info[i].bind = GATEWAY_NODE_BINDED;
                _gateway_subdev_cloud_info_store(i, &(g_gateway_subdev_cloud_info->info[i]));
            }
            break;
        }
    }

    /* sometimes the bind devid will be missed since the cloud_id has not been allocated */
    if (i == MAX_DEVICES_META_NUM + 1) {
        bind_missed_devid = devid;
    }
}

static void _gateway_subdev_delete_handle(int devid)
{
    int         index          = 0;
    gw_subdev_t subdev;

    subdev = _gateway_ut_get_subdev(devid);
    if (subdev <= 0) {
        gateway_err("delete: cloud id [%d] to subdev handle failed", devid);
    }

    gateway_subdev_del(subdev);

    /* delete cloud subdev info */
    index                                                = subdev;
    g_gateway_subdev_cloud_info->info[index].cloud_devid = GATEWAY_NODE_INVALID;
    g_gateway_subdev_cloud_info->info[index].bind        = GATEWAY_NODE_UNBIND;
    memset(&(g_gateway_subdev_cloud_info->info[index].meta), 0, sizeof(iotx_linkkit_dev_meta_info_t));

    _gateway_subdev_cloud_info_delete(index);
}

/**********************************************************************************************/

static gw_subdev_t _gateway_ut_get_subdev(int devid)
{
    gw_subdev_t subdev = -1;

    for (int i = 0; i < MAX_DEVICES_META_NUM; i++) {
        if (g_gateway_subdev_cloud_info->info[i].cloud_devid == devid) {
            subdev = i;
            break;
        }
    }

    return subdev;
}

gw_status_t app_gateway_ut_event_process(gw_event_type_e gw_evt, gw_evt_param_t gw_evt_param)
{
    switch (gw_evt) {
        case GW_SUBDEV_EVT_ADD: {
            gw_evt_subdev_add_t *gw_evt_subdev_add = (gw_evt_subdev_add_t *)gw_evt_param;
            subdev_cloud_info_t *info              = NULL;
            int16_t              index             = 0;

            if (gw_evt_subdev_add->status != 0) {
                gateway_warn("add subdev fail, status %d", gw_evt_subdev_add->status);
                return -1;
            }

            gateway_debug("add subdev[%d]", gw_evt_subdev_add->subdev);

            index = gw_evt_subdev_add->subdev;

            info              = &g_gateway_subdev_cloud_info->info[index];
            info->cloud_devid = GATEWAY_NODE_UNREG;
            info->subdev      = gw_evt_subdev_add->subdev;
            gateway_subdev_get_cloud_info(info->subdev);
            break;
        }
        case GW_SUBDEV_EVT_DEL: {
            gw_evt_subdev_del_t *gw_evt_subdev_del = (gw_evt_subdev_del_t *)gw_evt_param;
            int16_t              index             = 0;
            subdev_cloud_info_t *info              = NULL;

            gateway_debug("delete subdev[%d]", gw_evt_subdev_del->subdev);

            /* delete cloud subdev info */
            index = gw_evt_subdev_del->subdev;

            info = &g_gateway_subdev_cloud_info->info[index];
            if (info->cloud_devid != GATEWAY_NODE_INVALID) {
                gateway_reset_subdev(&info->meta);
                info->cloud_devid = GATEWAY_NODE_INVALID;
                info->bind        = GATEWAY_NODE_UNBIND;
                memset(&(info->meta), 0, sizeof(iotx_linkkit_dev_meta_info_t));

                _gateway_subdev_cloud_info_delete(index);
            }
            break;
        }
        default:
            break;
    }

    return 0;
}

gw_status_t app_gateway_ut_subdev_status_process(gw_subdev_t subdev, gw_subdev_status_type_e gw_status,
                                                 gw_status_param_t status, int status_len)
{
    switch (gw_status) {
        case GW_SUBDEV_STATUS_ONOFF: {
            int     devid                = 0;
            char    property_payload[64] = { 0 };
            int     res                  = 0;
            uint8_t pwrstate             = *(uint8_t *)status;

            gateway_debug("subdev[%d], onoff status %d", subdev, pwrstate);

            devid = g_gateway_subdev_cloud_info->info[subdev].cloud_devid;
            gateway_info("devid %d", devid);

            sprintf(property_payload, "{\"powerstate\":%d}", pwrstate);

            res = IOT_Linkkit_Report(devid, ITM_MSG_POST_PROPERTY, (unsigned char *)property_payload,
                                     strlen(property_payload));

            gateway_info("Post Property Message ID: %d, payload %s\n", res, property_payload);

            break;
        }
        case GW_SUBDEV_STATUS_BRI: {
            int      devid                = 0;
            char     property_payload[64] = { 0 };
            int      res                  = 0;
            uint16_t bri                  = *(uint16_t *)status;

            gateway_debug("subdev[%d], bri status %d", subdev, bri);

            devid = g_gateway_subdev_cloud_info->info[subdev].cloud_devid;
            gateway_info("devid %d", devid);

            sprintf(property_payload, "{\"brightness\":%d}", bri);

            res = IOT_Linkkit_Report(devid, ITM_MSG_POST_PROPERTY, (unsigned char *)property_payload,
                                     strlen(property_payload));

            gateway_info("Post Property Message ID: %d, payload %s\n", res, property_payload);

            break;
        }
        case GW_SUBDEV_STATUS_RAW_DATA: {
            int devid = 0;
            int res   = 0;

            gateway_debug("subdev[%d], raw data", subdev);

            devid = g_gateway_subdev_cloud_info->info[subdev].cloud_devid;
            gateway_info("devid %d", devid);

            res = IOT_Linkkit_Report(devid, ITM_MSG_POST_PROPERTY, (unsigned char *)status,
                                     strlen((const char *)status));

            gateway_info("Post Property Message ID: %d, payload %s\n", res, status);

            break;
        }
        case GW_SUBDEV_STATUS_MODEL: {
            int devid = 0;
            int res   = 0;

            gateway_debug("subdev[%d], model", subdev);

            devid = g_gateway_subdev_cloud_info->info[subdev].cloud_devid;
            gateway_info("devid %d", devid);

            res = IOT_Linkkit_Report(devid, ITM_MSG_POST_PROPERTY, (unsigned char *)status,
                                     strlen((const char *)status));

            gateway_info("Post Property Message ID: %d, payload %s\n", res, status);

            break;
        }
        case GW_SUBDEV_STATUS_TRIPLES_INFO: {
            gw_subdev_triples_t *         gw_occ_subdev_triples = (gw_subdev_triples_t *)status;
            iotx_linkkit_dev_meta_info_t *subdev_cloud_info     = NULL;
            int16_t                       index                 = subdev;

            subdev_cloud_info = &g_gateway_subdev_cloud_info->info[index].meta;

            memcpy(subdev_cloud_info->device_name, gw_occ_subdev_triples->device_name, DEVICE_NAME_LEN);
            memcpy(subdev_cloud_info->product_key, gw_occ_subdev_triples->product_key, PRODUCT_KEY_MAXLEN);
            memcpy(subdev_cloud_info->product_secret, gw_occ_subdev_triples->product_secret, PRODUCT_SECRET_MAXLEN);

            g_gateway_subdev_cloud_info->info[index].cloud_devid = GATEWAY_NODE_UNREG;

            _gateway_subdev_cloud_info_store(index, &(g_gateway_subdev_cloud_info->info[index]));

            break;
        }
        default:
            break;
    }

    return 0;
}

static int _gateway_ut_get_gw_triples(void)
{
    key_handle key_addr;
    int        ret                                   = KM_OK;
    int        len                                   = 0;
    int        product_id                            = 0;
    uint8_t    need_reload_from_kp                   = 0;
    char       product_key[PRODUCT_KEY_MAXLEN]       = { 0 };
    char       product_secret[PRODUCT_SECRET_MAXLEN] = { 0 };
    char       device_name[DEVICE_NAME_MAXLEN]       = { 0 };
    char       device_secret[DEVICE_SECRET_MAXLEN]   = { 0 };

    len = PRODUCT_KEY_MAXLEN;
    ret = aos_kv_get("hal_devinfo_pk", product_key, &len);
    if (ret != 0) {
        LOGD(TAG, "need reload triples from kp");
        need_reload_from_kp = 1;
        goto reload;
    }

    len = PRODUCT_SECRET_MAXLEN;
    ret = aos_kv_get("hal_devinfo_ps", product_secret, &len);
    if (ret != 0) {
        LOGD(TAG, "need reload triples from kp");
        need_reload_from_kp = 1;
        goto reload;
    }

    len = DEVICE_NAME_MAXLEN;
    ret = aos_kv_get("hal_devinfo_dn", device_name, &len);
    if (ret != 0) {
        LOGD(TAG, "need reload triples from kp");
        need_reload_from_kp = 1;
        goto reload;
    }

    len = DEVICE_SECRET_MAXLEN;
    ret = aos_kv_get("hal_devinfo_ds", device_secret, &len);
    if (ret != 0) {
        LOGD(TAG, "need reload triples from kp");
        need_reload_from_kp = 1;
        goto reload;
    }

    ret = aos_kv_getint("hal_devinfo_pid", &product_id);
    if (ret != 0) {
        LOGD(TAG, "need reload triples from kp");
        need_reload_from_kp = 1;
        goto reload;
    }

    if (need_reload_from_kp == 0) {
        LOGD(TAG, "already has triples, return");
        return 0;
    }

reload:
    ret = km_get_key(KEY_ID_PRODUCT_KEY, &key_addr, (uint32_t *)&len);
    if ((ret != KM_OK) || ((len > PRODUCT_KEY_MAXLEN))) {
        LOGE(TAG, "km get pk err or len too long, ret %d len %d", ret, len);
        return -1;
    }
    memcpy(product_key, (uint8_t *)key_addr, len);
    ret = aos_kv_set("hal_devinfo_pk", product_key, len, 1);
    if (ret != 0) {
        LOGE(TAG, "kv set fail");
        return -1;
    }
    LOGD(TAG, "PK: %s", product_key);

    ret = km_get_key(KEY_ID_PRODUCT_SECRET, &key_addr, (uint32_t *)&len);
    if ((ret != KM_OK) || ((len > PRODUCT_SECRET_MAXLEN))) {
        LOGE(TAG, "km get ps err or len too long, ret %d len %d", ret, len);
        return -1;
    }
    memcpy(product_secret, (uint8_t *)key_addr, len);
    ret = aos_kv_set("hal_devinfo_ps", product_secret, len, 1);
    if (ret != 0) {
        LOGE(TAG, "kv set fail");
        return -1;
    }
    LOGD(TAG, "PS: %s", product_secret);

    ret = km_get_key(KEY_ID_DEVICE_NAME, &key_addr, (uint32_t *)&len);
    if ((ret != KM_OK) || ((len > DEVICE_NAME_MAXLEN))) {
        LOGE(TAG, "km get dn err or len too long, ret %d len %d", ret, len);
        return -1;
    }
    memcpy(device_name, (uint8_t *)key_addr, len);
    ret = aos_kv_set("hal_devinfo_dn", device_name, len, 1);
    if (ret != 0) {
        LOGE(TAG, "kv set fail");
        return -1;
    }
    LOGD(TAG, "DN: %s", device_name);

    ret = km_get_key(KEY_ID_DEVICE_SECRET, &key_addr, (uint32_t *)&len);
    if ((ret != KM_OK) || ((len > DEVICE_SECRET_MAXLEN))) {
        LOGE(TAG, "km get ds err or len too long, ret %d len %d", ret, len);
        return -1;
    }
    memcpy(device_secret, (uint8_t *)key_addr, len);
    ret = aos_kv_set("hal_devinfo_ds", device_secret, len, 1);
    if (ret != 0) {
        LOGE(TAG, "kv set fail");
        return -1;
    }
    LOGD(TAG, "DS: %s", device_secret);

    ret = km_get_key(KEY_ID_PRODUCT_ID, &key_addr, (uint32_t *)&len);
    if ((ret != KM_OK) || ((len > PRODUCT_KEY_MAXLEN))) {
        LOGE(TAG, "km get pk err or len too long, ret %d len %d", ret, len);
        return -1;
    }
    product_id = *(int *)key_addr;
    ret        = aos_kv_setint("hal_devinfo_pid", product_id);
    if (ret != 0) {
        LOGE(TAG, "kv set fail");
        return -1;
    }
    LOGD(TAG, "PID: %d", product_id);

    return 0;
}

int app_gateway_ut_init(void)
{
    int                 ret                       = SUCCESS_RETURN;
    subdev_cloud_info_t subdev_cloud_info_from_kv = { 0 };

    ret = aos_queue_new(&gateway_queue_t, gateway_queue_buf, sizeof(gateway_queue_buf), QUEUE_MSG_SIZE);
    if (SUCCESS_RETURN != ret) {
        gateway_err("aos_queue_new failed");

        return ret;
    }

    g_gateway_subdev_cloud_info = aos_zalloc(sizeof(gw_subdev_cloud_info_t));
    if (g_gateway_subdev_cloud_info == NULL) {
        gateway_err("alloc g_gateway_subdev_cloud_info fail!!!");
        return FAIL_RETURN;
    }

    for (int i = 1; i < MAX_DEVICES_META_NUM + 1; i++) {
        g_gateway_subdev_cloud_info->info[i].cloud_devid = GATEWAY_NODE_INVALID;
    }
    g_gateway_subdev_cloud_info->info[0].cloud_devid = GATEWAY_NODE_UNREG;

    /* means there are init values stored in kv system*/
    for (int i = 0; i < MAX_DEVICES_META_NUM + 1; i++) {
        ret = _gateway_subdev_cloud_info_get(i, &subdev_cloud_info_from_kv);
        if (ret == 0) {
            memcpy(&g_gateway_subdev_cloud_info->info[i], &subdev_cloud_info_from_kv,
                   sizeof(subdev_cloud_info_from_kv));
            //gateway_info("dn[%d]: %s", i, g_gateway_subdev_cloud_info->info[i].meta.device_name);
        }
    }

    ret = _gateway_ut_get_gw_triples();
    if (ret != SUCCESS_RETURN) {
        gateway_err("gateway load smartliving triples fail");
        return ret;
    }

    return ret;
}

#endif
