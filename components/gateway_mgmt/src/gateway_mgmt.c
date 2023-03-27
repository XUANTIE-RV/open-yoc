/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#include <aos/kernel.h>
#include <aos/kv.h>
#include <aos/yloop.h>
#include <errno.h>

#include "ulog/ulog.h"
#include "gateway.h"
#include "gateway_mgmt.h"
#include "gateway_port.h"
#include "gateway_mesh.h"
#include "gateway_mgmt.h"
#include "gateway_mgmt_offline.h"
#include "gateway_occ_auth.h"
#include "gateway_mesh_model.h"
#include "gateway_occ_auth.h"
#include "gateway_ota.h"
#include "gateway_settings.h"

#define TAG "gateway_mgmt"

typedef struct {
    gw_subdev_t subdev;
} gateway_get_version_msg_t;

typedef union
{
    gateway_add_subdev_msg_t   add_msg;
    gateway_check_subdev_msg_t check_msg;
    gateway_get_version_msg_t  get_version_msg;
} gateway_mgmt_event_u;

typedef struct {
    uint8_t              event_type;
    gateway_mgmt_event_u mgmt_event;
} gateway_mgmt_event_process_t;

typedef struct {
    aos_timer_t get_node_ver_timer;
    uint8_t     get_node_ver_retry_cnt;
    uint16_t    get_node_ver_idx;
} gw_subdev_get_node_ver_ctx_t;

#define GW_SUB_DEV_QUEUE_MSG_SIZE sizeof(gateway_mgmt_event_process_t)
#define GW_SUB_DEV_QUEUE_MAX_SIZE (GW_SUB_DEV_QUEUE_MAX_NUM * GW_SUB_DEV_QUEUE_MSG_SIZE)

#if defined(CONFIG_SAVE_JS_TO_RAM) && (CONFIG_SAVE_JS_TO_RAM == 1)
extern aos_sem_t js_sem;
#endif

extern const char *bt_hex_real(const void *buf, size_t len);

typedef struct {
    uint8_t                      init_flag;
    aos_queue_t                  msg_queue;
    char *                       mgmt_event_queue_buf;
    gw_subdev_get_node_ver_ctx_t get_node_ver_ctx;
    gw_subdev_list_t *           subdev_ctx;
    gw_subgrp_list_t *           subgrp_ctx;
    gw_subdev_status_cb_t        subdev_status_cb;
    gw_event_cb_t                gateway_event_cb;
    gw_config_e                  config;
} gw_mgmt_ctx_t;

static gw_mgmt_ctx_t g_mgmt_ctx = { 0 };

static inline int16_t _gateway_subdev_get_index(gw_subdev_t subdev)
{
    return (int16_t)subdev;
}

static inline int16_t _gateway_subgrp_get_index(gw_subgrp_t subgrp)
{
    return (int16_t)subgrp;
}

static inline void mac_revert(uint8_t *mac, uint8_t *mac_converted)
{
    mac_converted[0] = mac[5];
    mac_converted[1] = mac[4];
    mac_converted[2] = mac[3];
    mac_converted[3] = mac[2];
    mac_converted[4] = mac[1];
    mac_converted[5] = mac[0];
}

static inline int _is_config_enabled(gw_config_e config)
{
    return g_mgmt_ctx.config & config;
}

gw_subdev_ctx_t *gateway_subdev_get_ctx(gw_subdev_t subdev)
{
    for (int i = 0; i < ARRAY_SIZE(g_mgmt_ctx.subdev_ctx->subdevs); i++) {
        if (g_mgmt_ctx.subdev_ctx->subdevs[i].subdev == subdev
            && g_mgmt_ctx.subdev_ctx->subdevs[i].state != GW_NODE_INVAL) {
            return &g_mgmt_ctx.subdev_ctx->subdevs[i];
        }
    }
    return NULL;
}

gw_subdev_ctx_t *gateway_subdev_get_ctx_by_index(uint16_t index)
{
    if (index >= ARRAY_SIZE(g_mgmt_ctx.subdev_ctx->subdevs)) {
        LOGE(TAG, "Invalid index %d", index);
        return NULL;
    }

    if (g_mgmt_ctx.subdev_ctx->subdevs[index].state != GW_NODE_INVAL) {
        return &g_mgmt_ctx.subdev_ctx->subdevs[index];
    } else {
        return NULL;
    }
}

gw_subdev_ctx_t *_gateway_subdev_get_free_ctx()
{
    /* 0 for gateway itself */
    for (int i = 1; i < ARRAY_SIZE(g_mgmt_ctx.subdev_ctx->subdevs); i++) {
        if (g_mgmt_ctx.subdev_ctx->subdevs[i].state == GW_NODE_INVAL) {
            g_mgmt_ctx.subdev_ctx->subdevs[i].subdev = i;
            return &g_mgmt_ctx.subdev_ctx->subdevs[i];
        }
    }
    return NULL;
}

gw_subgrp_ctx_t *_gateway_subgrp_get_ctx(gw_subgrp_t subgrp)
{
    for (int i = 0; i < ARRAY_SIZE(g_mgmt_ctx.subgrp_ctx->subgrps); i++) {
        if (g_mgmt_ctx.subgrp_ctx->subgrps[i].subgrp == subgrp
            && g_mgmt_ctx.subgrp_ctx->subgrps[i].state != GW_SUBGRP_INVAL) {
            return &g_mgmt_ctx.subgrp_ctx->subgrps[i];
        }
    }
    return NULL;
}

static inline char *_state_str(int state)
{
    switch (state) {
        case GW_NODE_INVAL:
            return "INVAL";
        case GW_NODE_ACTIVE:
            return "ACTIVE";
        case GW_NODE_ACTIVE_CHECK:
            return "ACTIVE_CHECK";
        case GW_NODE_NOT_ACTIVE:
            return "INACTIVE";
        default:
            return "UNKNOW";
    }
}

void gateway_subdev_state_update(gw_subdev_ctx_t *dev, int8_t state, uint8_t reason)
{
    if (!dev) {
        LOGE(TAG, "dev not set");
        return;
    }
    if (dev->state == state) {
        return;
    }

    LOGD(TAG, "Set Subdev %d: state:%s ---->%s", _gateway_subdev_get_index(dev->subdev), _state_str(dev->state),
         _state_str(state));
    if (g_mgmt_ctx.subdev_status_cb && (state == GW_NODE_ACTIVE || state == GW_NODE_NOT_ACTIVE)) {
        gw_status_subdev_t subdev_status;
        subdev_status.subdev = dev->subdev;
        if (state == GW_NODE_ACTIVE) {
            subdev_status.old_status = GW_SUBDEV_NOT_ACTIVE;
            subdev_status.new_status = GW_SUBDEV_ACTIVE;
        } else {
            subdev_status.old_status = GW_SUBDEV_ACTIVE;
            subdev_status.new_status = GW_SUBDEV_NOT_ACTIVE;
        }
        subdev_status.reason = reason;
        if (g_mgmt_ctx.subdev_status_cb) {
            g_mgmt_ctx.subdev_status_cb(dev->subdev, GW_SUBDEV_STATUS_CHANGE, (void *)&subdev_status,
                                        sizeof(gw_status_subdev_t));
        }
    }
    dev->state = state;

    gateway_subdev_ctx_store(dev);
}

int gateway_mgmt_send_process_msg(uint8_t event, void *msg)
{
    if (!msg) {
        LOGE(TAG, "Msg null");
        return -EINVAL;
    }

    gateway_mgmt_event_process_t process_msg;

    switch (event) {
        case GW_SUBDEV_ADD_EVENT: {
            memcpy(&process_msg.mgmt_event.add_msg, msg, sizeof(gateway_add_subdev_msg_t));
        } break;
        case GW_SUBDEV_ACTIVE_CHECK_EVENT: {
            memcpy(&process_msg.mgmt_event.check_msg, msg, sizeof(gateway_check_subdev_msg_t));
        } break;
        case GW_SUBDEV_VERSION_GET_EVENT: {
            memcpy(&process_msg.mgmt_event.get_version_msg, msg, sizeof(gateway_get_version_msg_t));
        } break;
        default:
            LOGE(TAG, "Unknow event %02x", event);
            return -1;
    }
    process_msg.event_type = event;
    // LOGD(TAG, "Send process msg:%d", event);
    return aos_queue_send(&g_mgmt_ctx.msg_queue, &process_msg, GW_SUB_DEV_QUEUE_MSG_SIZE);
}

gw_subdev_ctx_t *gateway_mgmt_get_subdev_ctx_by_mac(char *mac)
{
    gw_subdev_ctx_t *subdev_ctx = NULL;

    for (int i = 1; i < CONFIG_GW_MAX_DEVICES_META_NUM; i++) {
        subdev_ctx = &g_mgmt_ctx.subdev_ctx->subdevs[i];
        if (subdev_ctx->state == GW_NODE_INVAL)
            continue;
        switch (subdev_ctx->dev_type.protocol) {
            case GW_NODE_BLE_MESH: {
                if (!memcmp(mac, subdev_ctx->info.ble_mesh_info.dev_addr,
                            sizeof(subdev_ctx->info.ble_mesh_info.dev_addr))) {
                    return subdev_ctx;
                }
            } break;
            default:
                continue;
        }
    }
    return NULL;
}

static gw_subdev_ctx_t *_gateway_get_mesh_subdev_ctx(uint16_t unicast_addr)
{
    for (int i = 0; i < ARRAY_SIZE(g_mgmt_ctx.subdev_ctx->subdevs); i++) {
        if (g_mgmt_ctx.subdev_ctx->subdevs[i].state != GW_NODE_INVAL
            && g_mgmt_ctx.subdev_ctx->subdevs[i].dev_type.protocol == GW_NODE_BLE_MESH)
        {
            gw_subdev_ble_mesh_info_t *info = &g_mgmt_ctx.subdev_ctx->subdevs[i].info.ble_mesh_info;
            if (unicast_addr >= info->unicast_addr && unicast_addr <= info->unicast_addr + info->elem_num - 1) {
                return &g_mgmt_ctx.subdev_ctx->subdevs[i];
            }
        }
    }
    return NULL;
}

#if defined(CONFIG_OCC_AUTH) && (CONFIG_OCC_AUTH)
static int _gateway_mgmt_subdev_get_occ_cid(gateway_add_subdev_msg_t *dev_info, uint8_t *cid)
{
    int           ret           = 0;
    gw_auth_in_t  auth_in_info  = { 0x0 };
    gw_auth_out_t auth_out_info = { 0x0 };

    if (!_is_config_enabled(GW_CONFIG_SUPPORT_OCC_AUTH)) {
        LOGD(TAG, "OCC AUTH not enable");
        return 0;
    }

    if (dev_info->dev_type.protocol == GW_NODE_BLE_MESH) {
        auth_in_info.dev_protocal                       = GW_OCC_AUTH_TYPE_MESH;
        auth_in_info.in_data.mesh_occ_in_data.addr_type = dev_info->protocol_info.ble_mesh_info.addr_type;
        memcpy(auth_in_info.in_data.mesh_occ_in_data.addr, dev_info->protocol_info.ble_mesh_info.dev_addr, 6);
        memcpy(auth_in_info.in_data.mesh_occ_in_data.uuid, dev_info->protocol_info.ble_mesh_info.uuid, 16);
        ret = gateway_occ_node_auth_result_get(auth_in_info, &auth_out_info, GW_DEF_OCC_AUTH_NODE_TIMEOUT);
        if (!ret && auth_out_info.auth_status == OCC_AUTH_SUCCESS) {
            strncpy((char *)cid, auth_out_info.out_data.mesh_occ_out_data.CID,
                    strlen(auth_out_info.out_data.mesh_occ_out_data.CID));
        } else {
            LOGE(TAG, "Occ auth get node result failed %d", ret);
            return ret;
        }
        ret = gateway_occ_node_auth_cache_remove(auth_in_info);
        if (ret) {
            LOGW(TAG, "Occ auth node remove failed %d", ret);
        }
    } else {
        LOGE(TAG, "Unsupport occ auth protocal %02x", dev_info->dev_type.protocol);
        return -EOPNOTSUPP;
    }
    return 0;
}

static int _gateway_mgmt_subdev_get_triples_and_js_by_cloud(uint8_t *cid, gw_subdev_triples_t *triples)
{
    int ret = 0;
    /* get pid*/
#if defined(CONFIG_OCC_AUTH) && (CONFIG_OCC_AUTH)
    ret = gateway_occ_get_dev_triples(cid, triples);
    if (ret != 0) {
        LOGE(TAG, "Occ get triple failed %d", ret);
        return ret;
    }
#endif

#if defined(CONFIG_TSL_DEVICER_MESH) && defined(CONFIG_SAVE_JS_TO_RAM) && (CONFIG_SAVE_JS_TO_RAM == 1)
    if (!_is_config_enabled(GW_CONFIG_SUPPORT_TSL_ENGINE)) {
        return 0;
    }

    if (!is_js_already_existed(triples->occ_pid)) {
        return gateway_occ_getjs(triples->occ_pid);
    }
#endif
    return 0;
}
#endif

static void _gateway_mgmt_load_subdev(gw_subdev_ctx_t *subdev_ctx, gateway_add_subdev_msg_t *dev_info)
{
    subdev_ctx->period             = gateway_default_offline_check_timeout_get(dev_info->dev_type.lpm_type);
    subdev_ctx->hb_last_recv       = aos_now_ms();
    subdev_ctx->dev_type.lpm_type  = dev_info->dev_type.lpm_type;
    subdev_ctx->dev_type.protocol  = dev_info->dev_type.protocol;
    subdev_ctx->dev_type.node_type = dev_info->dev_type.node_type;
    memcpy(subdev_ctx->info.ble_mesh_info.dev_addr, dev_info->protocol_info.ble_mesh_info.dev_addr,
           6); // Todo for other protocal
    subdev_ctx->info.ble_mesh_info.addr_type    = dev_info->protocol_info.ble_mesh_info.addr_type;
    subdev_ctx->info.ble_mesh_info.unicast_addr = dev_info->protocol_info.ble_mesh_info.unicast_addr;
    subdev_ctx->info.ble_mesh_info.elem_num     = dev_info->protocol_info.ble_mesh_info.elem_num;
    LOGD(TAG, "Subdev [%d] added, Protocol type:%d,lpm type:%d,node type:%d",
         _gateway_subdev_get_index(subdev_ctx->subdev), subdev_ctx->dev_type.protocol, subdev_ctx->dev_type.lpm_type,
         subdev_ctx->dev_type.node_type);
}

static int _gateway_mgmt_subdev_simple_rst(gw_subdev_type_t type, gw_subdev_info_t info)
{
    switch (type.protocol) {
        case GW_NODE_BLE_MESH: {
            return gateway_porting_dev_simple_rst(GW_NODE_BLE_MESH, &info.ble_mesh_info.unicast_addr);
        } break;
        default:
            return -EOPNOTSUPP;
    }
}

static void _gateway_mgmt_subdev_add(gateway_add_subdev_msg_t *dev_info, uint8_t retry_time)
{
    int                 ret               = 0;
    gw_subdev_ctx_t *   subdev_ctx        = NULL;
    gw_evt_subdev_add_t gw_evt_subdev_add = { 0 };
    uint8_t             add_status        = 0;
#if defined(CONFIG_OCC_AUTH) && (CONFIG_OCC_AUTH)
    gateway_add_subdev_msg_t msg     = { 0 };
    gw_subdev_triples_t      triples = { 0x0 };
    char                     cid[33] = { 0 };
#endif

    if (dev_info->dev_type.node_type == GW_DEV_GATEWAY) {
        subdev_ctx         = &g_mgmt_ctx.subdev_ctx->subdevs[0];
        subdev_ctx->subdev = 0;
        _gateway_mgmt_load_subdev(subdev_ctx, dev_info);
        gateway_subdev_state_update(subdev_ctx, GW_NODE_ACTIVE, GW_SUBDEV_STATUS_CHANGE_BY_NODE_ADD);
        return;
    }

    switch (dev_info->dev_type.protocol) {
        case GW_NODE_BLE_MESH: {
            subdev_ctx = _gateway_get_mesh_subdev_ctx(dev_info->protocol_info.ble_mesh_info.unicast_addr);
            if (subdev_ctx) {
                LOGD(TAG, "Dev already exist");
                return;
            }
        } break;
        default:
            add_status = GW_NODE_BIND_FAILED_REASON_PROTOCOL_NOT_SUPPORT;
            goto add_finish;
    }

    subdev_ctx = _gateway_subdev_get_free_ctx();
    if (!subdev_ctx) {
        LOGE(TAG, "No free ctx");
        add_status = GW_NODE_BIND_FAILED_REASON_NO_BUFFER;
        goto add_finish;
    }

#if defined(CONFIG_OCC_AUTH) && (CONFIG_OCC_AUTH)
    ret = _gateway_mgmt_subdev_get_occ_cid(dev_info, (uint8_t *)cid);
    if (ret) {
        LOGE(TAG, "Get occ pid failed %d", ret);
        add_status = GW_NODE_BIND_FAILED_REASON_OCC_AUTH_FAILED;
        goto add_dev_retry;
    }
    ret = _gateway_mgmt_subdev_get_triples_and_js_by_cloud((uint8_t *)cid, &triples);
    if (ret) {
        LOGE(TAG, "Get triples failed %d", ret);
        add_status = GW_NODE_BIND_FAILED_REASON_GET_JS_FAILED;
        goto add_dev_retry;
    }
#endif

    add_status = GW_NODE_BIND_SUCCESS;
    _gateway_mgmt_load_subdev(subdev_ctx, dev_info);

    goto add_finish;

#if defined(CONFIG_OCC_AUTH) && (CONFIG_OCC_AUTH)
add_dev_retry:
    memcpy(&msg, dev_info, sizeof(gateway_add_subdev_msg_t));
    msg.retry_time = retry_time + 1;
    if (msg.retry_time < GW_ADD_SUBDEV_MAX_RETRY_TIME) {
        gateway_mgmt_send_process_msg(GW_SUBDEV_ADD_EVENT, (gateway_add_subdev_msg_t *)&msg);
        return;
    } else {
        LOGE(TAG, "Sub dev add faileded");
        goto add_finish;
    }
    return;
#endif

add_finish:
    /* rpt to application*/
    gw_evt_subdev_add.status = add_status;
    if (add_status == GW_NODE_BIND_SUCCESS) {
#if defined(CONFIG_OCC_AUTH) && (CONFIG_OCC_AUTH)
        strncpy(subdev_ctx->cid, cid, strlen(cid));
        strncpy(subdev_ctx->occ_pid, triples.occ_pid, strlen(triples.occ_pid));
#endif
        gateway_subdev_state_update(subdev_ctx, GW_NODE_ACTIVE, GW_SUBDEV_STATUS_CHANGE_BY_NODE_ADD);
        memcpy(&gw_evt_subdev_add.subdev, &subdev_ctx->subdev, sizeof(gw_subdev_t));
    } else {
        ret = _gateway_mgmt_subdev_simple_rst(dev_info->dev_type, dev_info->protocol_info);
        if (ret) {
            LOGW(TAG, "Subdev simple rst failed %d", ret);
        }
    }

    if (g_mgmt_ctx.gateway_event_cb) {
        g_mgmt_ctx.gateway_event_cb(GW_SUBDEV_EVT_ADD, (gw_evt_param_t)&gw_evt_subdev_add);
    }

    if (add_status == GW_NODE_BIND_SUCCESS) {
        ret = aos_timer_stop(&g_mgmt_ctx.get_node_ver_ctx.get_node_ver_timer);
        if (ret) {
            LOGE(TAG, "Version get timer stop failed", ret);
        }
        ret = aos_timer_start(&g_mgmt_ctx.get_node_ver_ctx.get_node_ver_timer);
        if (ret) {
            LOGE(TAG, "Version get timer start failed", ret);
        }
        g_mgmt_ctx.get_node_ver_ctx.get_node_ver_retry_cnt = 0;

#if defined(CONFIG_OCC_AUTH) && (CONFIG_OCC_AUTH)
        if (g_mgmt_ctx.subdev_status_cb) {
            g_mgmt_ctx.subdev_status_cb(subdev_ctx->subdev, GW_SUBDEV_STATUS_TRIPLES_INFO, (gw_status_param_t)&triples,
                                        sizeof(gw_subdev_triples_t));
        }
#endif
    }
    return;
}

static void _gateway_mgmt_subdev_del(gw_subdev_rst_status *rst_status)
{
    gw_subdev_ctx_t *   subdev_ctx = NULL;
    gw_evt_subdev_del_t gw_evt     = { 0 };

    switch (rst_status->protocol) {
        case GW_NODE_BLE_MESH: {
            subdev_ctx = _gateway_get_mesh_subdev_ctx(rst_status->protocol_info.ble_mesh_info.unicast_addr);
            if (!subdev_ctx) {
                LOGE(TAG, "Get mesh Subdev failed");
                return;
            }
        } break;
        default:
            LOGE(TAG, "Nnknow protocal for subdev status");
            return;
    }
    memcpy(&gw_evt.subdev, &subdev_ctx->subdev, sizeof(gw_subdev_t));
    gw_evt.status = 0;
    if (g_mgmt_ctx.gateway_event_cb) {
        g_mgmt_ctx.gateway_event_cb(GW_SUBDEV_EVT_DEL, (gw_evt_param_t)&gw_evt);
    }
    gateway_subdev_state_update(subdev_ctx, GW_NODE_INVAL, GW_SUBDEV_STATUS_CHANGE_BY_NODE_RST);

    gateway_subdev_ctx_delete(subdev_ctx);
}

#if defined(CONFIG_OCC_AUTH) && (CONFIG_OCC_AUTH)
static void _gateway_occ_auth_cb(gw_auth_out_t *out_info)
{
    if (!_is_config_enabled(GW_CONFIG_SUPPORT_OCC_AUTH)) {
        return;
    }

    if (!out_info) {
        return;
    }

    switch (out_info->auth_status) {
        case OCC_AUTH_FAILED: {
            // LOGD(TAG,"Occ auth failed, using protocol %02x",out_info->dev_protocal );
            return;
        } break;

        case OCC_AUTH_SUCCESS: {
            gw_evt_discovered_info_t discovered_msg = { 0 };
            if (out_info->dev_protocal == GW_OCC_AUTH_TYPE_MESH) {
                memcpy(discovered_msg.protocol_info.ble_mesh_info.dev_addr, out_info->out_data.mesh_occ_out_data.addr,
                       6);
                memcpy(discovered_msg.protocol_info.ble_mesh_info.uuid, out_info->out_data.mesh_occ_out_data.uuid, 16);
                discovered_msg.protocol                              = GW_NODE_BLE_MESH;
                discovered_msg.protocol_info.ble_mesh_info.addr_type = out_info->out_data.mesh_occ_out_data.addr_type;
                discovered_msg.protocol_info.ble_mesh_info.bearer    = out_info->out_data.mesh_occ_out_data.bearer;
                discovered_msg.protocol_info.ble_mesh_info.oob_info  = out_info->out_data.mesh_occ_out_data.oob_info;
            } else {
                LOGW(TAG, "Ignore the occ auth satus, using protocol %02x", out_info->dev_protocal);
            }
            if (g_mgmt_ctx.gateway_event_cb) {
                g_mgmt_ctx.gateway_event_cb(GW_EVT_FOUND_DEV, (gw_evt_param_t)&discovered_msg);
            }
        } break;
    }
}
#endif

static void _gateway_subdev_version_recv(gw_subdev_ctx_t *subdev_ctx, gw_subdev_version_status version_status)
{
    char version_str[14] = { 0 };

    uint32_t version          = version_status.version;
    uint8_t  support_protocol = version_status.support_protocol;

    snprintf(version_str, sizeof(version_str) - 1, "%d.%d.%d", (version >> 16) & 0xff, (version >> 8) & 0xff,
             (version >> 0) & 0xff);
    LOGD(TAG, "Subdev:%d,version:%s,Support ota protocol %02x, State:%02x",
         _gateway_subdev_get_index(subdev_ctx->subdev), version_str, support_protocol, subdev_ctx->state);

    subdev_ctx->version              = version;
    subdev_ctx->support_ota_protocol = support_protocol;

    gateway_subdev_ctx_store(subdev_ctx);

    if (subdev_ctx->state == GW_NODE_ACTIVE_CHECK || subdev_ctx->state == GW_NODE_NOT_ACTIVE) {
        gateway_subdev_state_update(subdev_ctx, GW_NODE_ACTIVE, GW_SUBDEV_STATUS_CHANGE_BY_VERSION_RECV);
    }

    if (g_mgmt_ctx.subdev_status_cb) {
        g_mgmt_ctx.subdev_status_cb(subdev_ctx->subdev, GW_SUBDEV_STATUS_VERSION, NULL, 0);
    }
}

static void _gateway_mgmt_scan_result(gw_evt_discovered_info_t *msg)
{
#if defined(CONFIG_OCC_AUTH) && (CONFIG_OCC_AUTH)
    if (_is_config_enabled(GW_CONFIG_SUPPORT_OCC_AUTH)) {
        gw_auth_in_t auth_in_info;
        uint8_t      auth_status;
        if (msg->protocol != GW_NODE_BLE_MESH) {
            LOGE(TAG, "occ auth input not support protocol %02x", msg->protocol);
            return;
        }

        memcpy(&auth_in_info.in_data.mesh_occ_in_data.addr, msg->protocol_info.ble_mesh_info.dev_addr, 6);
        memcpy(&auth_in_info.in_data.mesh_occ_in_data.uuid, msg->protocol_info.ble_mesh_info.uuid, 16);
        auth_in_info.dev_protocal                       = GW_OCC_AUTH_TYPE_MESH;
        auth_in_info.in_data.mesh_occ_in_data.addr_type = msg->protocol_info.ble_mesh_info.addr_type;
        auth_in_info.in_data.mesh_occ_in_data.bearer    = msg->protocol_info.ble_mesh_info.bearer;
        auth_in_info.in_data.mesh_occ_in_data.oob_info  = msg->protocol_info.ble_mesh_info.oob_info;
        gateway_occ_node_auth(auth_in_info, &auth_status);
        if (auth_status != OCC_AUTH_SUCCESS) {
            LOGD(TAG, "Dev:%s occ auth status not success, will not report it",
                 bt_hex_real(msg->protocol_info.ble_mesh_info.dev_addr, 6));
            return;
        }
    }
#endif

    if (g_mgmt_ctx.gateway_event_cb) {
        g_mgmt_ctx.gateway_event_cb(GW_EVT_FOUND_DEV, (gw_evt_param_t)msg);
    }
}

static void _gateway_mgmt_scan_timeout(void)
{
    if (g_mgmt_ctx.gateway_event_cb) {
        g_mgmt_ctx.gateway_event_cb(GW_EVT_FOUND_DEV_TIMEOUT, NULL);
    }
}

static void _gateway_mgmt_mesh_auth_input(gw_mesh_node_auth_ctx_t *ctx)
{
#if defined(CONFIG_OCC_AUTH) && (CONFIG_OCC_AUTH)
    int ret = 0;
    if (_is_config_enabled(GW_CONFIG_SUPPORT_OCC_AUTH)) {
        gw_auth_in_t  auth_in_info  = { 0x0 };
        gw_auth_out_t auth_out_info = { 0x0 };
        memcpy(auth_in_info.in_data.mesh_occ_in_data.addr, ctx->dev_addr, 6);
        memcpy(auth_in_info.in_data.mesh_occ_in_data.uuid, ctx->uuid, 16);
        auth_in_info.in_data.mesh_occ_in_data.addr_type = ctx->addr_type;
        auth_in_info.dev_protocal                       = GW_OCC_AUTH_TYPE_MESH;
        ret = gateway_occ_node_auth_result_get(auth_in_info, &auth_out_info, GW_DEF_OCC_AUTH_NODE_TIMEOUT);
        if (ret) {
            LOGE(TAG, "mesh auth result get failed %d", ret);
        } else {
            ret = gateway_mesh_auth_input(ctx->action, &auth_out_info.out_data.mesh_occ_out_data.short_oob,
                                          sizeof(auth_out_info.out_data.mesh_occ_out_data.short_oob));
            if (ret) {
                LOGE(TAG, "mesh occ auth input failed %d", ret);
            }
        }
    }
#else
    LOGW(TAG, "Mesh auth input ot support");
#endif
}

static void _gateway_mgmt_auth_input(gw_auth_input_t *msg)
{
    switch (msg->procotol) {
        case GW_NODE_BLE_MESH: {
            _gateway_mgmt_mesh_auth_input(&msg->auth_ctx.mesh_ctx);
        } break;
        default:
            LOGE(TAG, "Auth not support for the protocol %02x", msg->procotol);
    }
}

static void _gateway_mgmt_subdev_add_fail(gw_evt_bind_fail_t *msg)
{
    gw_evt_subdev_add_fail_t fail_msg = { 0x0 };

#if defined(CONFIG_OCC_AUTH) && (CONFIG_OCC_AUTH)
    int ret = 0;
    if (_is_config_enabled(GW_CONFIG_SUPPORT_OCC_AUTH)) {
        gw_auth_in_t in_info = { 0x0 };
        if (msg->protocol == GW_NODE_BLE_MESH) {
            in_info.dev_protocal = GW_OCC_AUTH_TYPE_MESH;
            memcpy(in_info.in_data.mesh_occ_in_data.addr, msg->protocol_info.ble_mesh_info.dev_addr, 6);
            memcpy(in_info.in_data.mesh_occ_in_data.uuid, msg->protocol_info.ble_mesh_info.uuid, 16);
            in_info.in_data.mesh_occ_in_data.addr_type = msg->protocol_info.ble_mesh_info.addr_type;

            ret = gateway_occ_node_auth_cache_remove(in_info);
            if (ret) {
                LOGE(TAG, "Occ auth node remove fild %d", ret);
            }
        } else {
            LOGD(TAG, "Unsupport occ auth protocol %02x", msg->protocol);
        }
    }
#endif
    fail_msg.protocol      = msg->protocol;
    fail_msg.failed_reason = msg->failed_reason;
    memcpy(&fail_msg.protocol_info, &msg->protocol_info, sizeof(gw_discovered_dev_protocol_info_t));

    if (g_mgmt_ctx.gateway_event_cb) {
        g_mgmt_ctx.gateway_event_cb(GW_SUBDEV_EVT_ADD_FAIL, (gw_evt_param_t)&fail_msg);
    }
}

static int _gateway_mgmt_update_power_state(gw_subdev_ctx_t *subdev_ctx, uint8_t onoff)
{
    if (g_mgmt_ctx.subdev_status_cb) {
        g_mgmt_ctx.subdev_status_cb(subdev_ctx->subdev, GW_SUBDEV_STATUS_ONOFF, (void *)&onoff, sizeof(onoff));
    }

    return 0;
}

static int _gateway_mgmt_update_level_state(gw_subdev_ctx_t *subdev_ctx, uint16_t level)
{
    if (g_mgmt_ctx.subdev_status_cb) {
        g_mgmt_ctx.subdev_status_cb(subdev_ctx->subdev, GW_SUBDEV_STATUS_LEVEL, (void *)&level, sizeof(level));
    }

    return 0;
}

static int _gateway_mgmt_update_brightness(gw_subdev_ctx_t *subdev_ctx, uint16_t bri)
{
    if (g_mgmt_ctx.subdev_status_cb) {
        g_mgmt_ctx.subdev_status_cb(subdev_ctx->subdev, GW_SUBDEV_STATUS_BRI, (void *)&bri, sizeof(bri));
    }

    return 0;
}

static int _gateway_mgmt_update_cct(gw_subdev_ctx_t *subdev_ctx, uint16_t cct)
{
    if (g_mgmt_ctx.subdev_status_cb) {
        g_mgmt_ctx.subdev_status_cb(subdev_ctx->subdev, GW_SUBDEV_STATUS_CCT, (void *)&cct, sizeof(cct));
    }
    return 0;
}

static void _gateway_mgmt_hb_mgmt_set_status(gw_subdev_hb_mgmt_set_status_t *msg)
{
    if (g_mgmt_ctx.gateway_event_cb) {
        g_mgmt_ctx.gateway_event_cb(GW_SUBDEV_HB_MGMT_SET, (gw_evt_param_t)msg);
    }
}

static void _gateway_mgmt_triples_status(gw_subdev_ctx_t *subdev_ctx, gw_occ_subdev_triples_status_t triples_status)
{
    gw_subdev_triples_t triples = { 0 };

    if (triples_status.status) {
        LOGE(TAG, "Get triples failed");
        return;
    } else {
        LOGD(TAG, "Get triples success, flag:%02x", triples_status.flag);
    }

    memcpy(&triples, &triples_status.status_data, sizeof(gw_subdev_triples_t));
    if (!(triples_status.flag & TRIPLE_TYPE_DN)) {
        LOGD(TAG, "Dn not get,set by local");
		snprintf(triples.device_name, sizeof(triples.device_name) -1, "%02x%02x%02x%02x%02x%02x", \
                subdev_ctx->info.ble_mesh_info.dev_addr[5], subdev_ctx->info.ble_mesh_info.dev_addr[4], \
                subdev_ctx->info.ble_mesh_info.dev_addr[3], subdev_ctx->info.ble_mesh_info.dev_addr[2], \
                subdev_ctx->info.ble_mesh_info.dev_addr[1], subdev_ctx->info.ble_mesh_info.dev_addr[0]);
        LOGD(TAG, "dn:%s", triples.device_name);
    } else {
        LOGD(TAG, "Subdev:%d,dn:%s", subdev_ctx->subdev, triples.device_name);
    }
    LOGD(TAG, "Subdev:%d,pk:%s", subdev_ctx->subdev, triples.product_key);
    LOGD(TAG, "Subdev:%d,ps:%s", subdev_ctx->subdev, triples.product_secret);

    if (g_mgmt_ctx.subdev_status_cb) {
        g_mgmt_ctx.subdev_status_cb(subdev_ctx->subdev, GW_SUBDEV_STATUS_TRIPLES_INFO, (gw_status_param_t)&triples,
                                    sizeof(gw_subdev_triples_t));
    }
}

static int _gateway_mgmt_raw_data_cb(gw_subdev_ctx_t *subdev_ctx, uint8_t *data, size_t data_len)
{
    if (g_mgmt_ctx.subdev_status_cb) {
        return g_mgmt_ctx.subdev_status_cb(subdev_ctx->subdev, GW_SUBDEV_STATUS_RAW_DATA, (gw_status_param_t)data,
                                           data_len);
    }
    return 0;
}

int gateway_mgmt_modem_status(gw_subdev_ctx_t *subdev_ctx, uint8_t *data, size_t data_len)
{
    if (g_mgmt_ctx.subdev_status_cb) {
        return g_mgmt_ctx.subdev_status_cb(subdev_ctx->subdev, GW_SUBDEV_STATUS_MODEL, (gw_status_param_t)data,
                                           data_len);
    }
    return 0;
}

static int _gateway_mgmt_update_event_cb(void *msg, int msg_len, enum_mgmt_msg_type_e msg_type)
{
    switch (msg_type) {
        case GW_MSG_ADD_SUB_DEV: {
            gateway_mgmt_send_process_msg(GW_SUBDEV_ADD_EVENT, (gateway_add_subdev_msg_t *)msg);
        } break;
        case GW_MSG_DEL_SUB_DEV: {
            _gateway_mgmt_subdev_del((gw_subdev_rst_status *)msg);
        } break;
        case GW_MSG_SCAN_DEV: {
            _gateway_mgmt_scan_result(msg);
        } break;
        case GW_MSG_SCAN_DEV_TIMEOUT: {
            _gateway_mgmt_scan_timeout();
        } break;
        case GW_MSG_ADD_SUB_DEV_FAIL: {
            _gateway_mgmt_subdev_add_fail((gw_evt_bind_fail_t *)msg);
        } break;
        case GW_MSG_AUTH_INPUT: {
            _gateway_mgmt_auth_input((gw_auth_input_t *)msg);
        } break;
        case GW_MSG_DEV_HB_MGMT_SET: {
            _gateway_mgmt_hb_mgmt_set_status((gw_subdev_hb_mgmt_set_status_t *)msg);
        } break;
        default:
            LOGD(TAG, "Unknow gw mgmt msg type %02x", msg_type);
            return -1;
    }

    return 0;
}

static gw_subdev_ctx_t *_gateway_mgmt_get_subdev_by_dev_info(gw_subdev_status_update_dev_info *dev_info)
{

    switch (dev_info->protocol) {
        case GW_NODE_BLE_MESH: {
            return _gateway_get_mesh_subdev_ctx(dev_info->protocol_info.ble_mesh_info.unicast_addr);
        } break;
        default:
            return NULL;
    }

    return NULL;
}

static int _gateway_mgmt_subdev_get_mac(gw_subdev_ctx_t *subdev_ctx, char *mac)
{
    switch (subdev_ctx->dev_type.protocol) {
        case GW_NODE_BLE_MESH: {
            memcpy(mac, subdev_ctx->info.ble_mesh_info.dev_addr, sizeof(subdev_ctx->info.ble_mesh_info.dev_addr));
        } break;
        default:
            LOGE(TAG, "No mac addr for protocol %02x", subdev_ctx->dev_type.protocol);
            return -EOPNOTSUPP;
    }
    return 0;
}

static int _gateway_mgmt_update_status_cb(gw_subdev_status_update_dev_info *dev_info, void *status, int status_len,
                                          enum_subdev_status_type_e status_type)
{
    int              ret        = 0;
    gw_subdev_ctx_t *subdev_ctx = NULL;
    subdev_ctx                  = _gateway_mgmt_get_subdev_by_dev_info(dev_info);
    if (!subdev_ctx) {
        return -EINVAL;
    }

    switch (status_type) {
#if defined(CONFIG_GATEWAY_SUBDEV_OFFLINE_CHECK) && CONFIG_GATEWAY_SUBDEV_OFFLINE_CHECK > 0
        case GW_SUBDEV_HB_SET_STATUS: {
            ret = gateway_subdev_hb_pub_status_update(subdev_ctx, *(gw_subdev_hb_set_status_t *)status);
            if (!ret && g_mgmt_ctx.subdev_status_cb) {
                g_mgmt_ctx.subdev_status_cb(subdev_ctx->subdev, GW_SUBDEV_STATUS_HB_SET, status,
                                            sizeof(gw_subdev_hb_set_status_t));
            }
        } break;
        case GW_SUBDEV_HB_STATUS: {
            gateway_subdev_hb_recv(subdev_ctx);
        } break;
        case GW_SUBDEV_ACTIVE_CHECK_STATUS: {
            gateway_subdev_active_status_recv(subdev_ctx, *(gw_active_check_status_t *)status);
        } break;
#endif
        case GW_SUBDEV_VERSION_STATUS: {
            _gateway_subdev_version_recv(subdev_ctx, *(gw_subdev_version_status *)status);
        } break;
        case GW_SUBDEV_ONOFF_STATUS: {
            _gateway_mgmt_update_power_state(subdev_ctx, *(uint8_t *)status);
        } break;
        case GW_SUBDEV_LEVEL_STATUS: {
            _gateway_mgmt_update_level_state(subdev_ctx, *(uint16_t *)status);
        } break;
        case GW_SUBDEV_BRIGHTNESS_STATUS: {
            _gateway_mgmt_update_brightness(subdev_ctx, *(uint16_t *)status);
        } break;
        case GW_SUBDEV_CCT_STATUS: {
            _gateway_mgmt_update_cct(subdev_ctx, *(uint16_t *)status);
        } break;
        case GW_SUBDEV_TRIPLES_STATUS: {
            _gateway_mgmt_triples_status(subdev_ctx, *(gw_occ_subdev_triples_status_t *)status);
        } break;
        case GW_SUBDEV_RAW_DATA: {
            _gateway_mgmt_raw_data_cb(subdev_ctx, (uint8_t *)status, status_len);
        } break;
        default:
            LOGD(TAG, "Unknow subdev status type %02x", status_type);
    }

    (void)ret;
    return 0;
}

static void _gateway_mgmt_event_process_task(void *arg)
{
    int                          ret = 0;
    uint32_t                     len;
    gateway_mgmt_event_process_t msg;

    while (1) {
        aos_queue_recv(&g_mgmt_ctx.msg_queue, AOS_WAIT_FOREVER, &msg, (size_t *)&len);
        LOGD(TAG, "Process msg event %d", msg.event_type);
        switch (msg.event_type) {
            case GW_SUBDEV_ADD_EVENT: {
                gateway_add_subdev_msg_t *add_msg = &msg.mgmt_event.add_msg;
                if (add_msg->retry_time < GW_ADD_SUBDEV_MAX_RETRY_TIME) {
                    _gateway_mgmt_subdev_add(add_msg, add_msg->retry_time);
                }
            } break;
            case GW_SUBDEV_ACTIVE_CHECK_EVENT: {
                gateway_check_subdev_msg_t *check_msg = &msg.mgmt_event.check_msg;
                gw_subdev_ctx_t *           dev       = gateway_subdev_get_ctx(check_msg->subdev);
                if (!dev) {
                    LOGE(TAG, "dev not found for index %d", check_msg->subdev);
                } else {
                    ret = gateway_mesh_hb_config_check(dev->info.ble_mesh_info.unicast_addr, check_msg->period);
                    if (ret) {
                        LOGE(TAG, "actice check send failed for index %d %d", check_msg->subdev, ret);
                    }
                }
            } break;
            case GW_SUBDEV_VERSION_GET_EVENT: {
                gateway_get_version_msg_t *get_version_msg = &msg.mgmt_event.get_version_msg;
                gw_subdev_ctx_t *          dev             = gateway_subdev_get_ctx(get_version_msg->subdev);
                if (!dev) {
                    LOGE(TAG, "dev not found for index %d", get_version_msg->subdev);
                } else {
                    ret = gateway_mesh_node_version_get(dev->info.ble_mesh_info.unicast_addr);
                    if (ret) {
                        LOGE(TAG, "version get failed for index %d", get_version_msg->subdev);
                    }
                }
            } break;
            default:
                LOGE(TAG, "Unknow mgmt event %d", msg.event_type);
        }
    }
}

#if defined(CONFIG_OCC_AUTH) && (CONFIG_OCC_AUTH)
int gateway_mgmt_get_mesh_subdev_occpid(gw_subdev_t subdev, char *occ_pid)
{
    gw_subdev_ctx_t *subdev_ctx = NULL;
    subdev_ctx                  = gateway_subdev_get_ctx(subdev);
    if (!subdev_ctx) {
        LOGE(TAG, "Get mesh subdev ctx faileded");
        return -EIO;
    }
    strncpy(occ_pid, subdev_ctx->occ_pid, strlen(subdev_ctx->occ_pid));
    return 0;
}
#endif

#if defined(CONFIG_OCC_AUTH) && (CONFIG_OCC_AUTH)
#if defined(CONFIG_SAVE_JS_TO_RAM) && (CONFIG_SAVE_JS_TO_RAM == 1)
static void _gateway_mgmt_get_occ_js(void *p)
{
    for (int i = 1; i < CONFIG_GW_MAX_DEVICES_META_NUM + 1; i++) {
        if (g_mgmt_ctx.subdev_ctx->subdevs[i].state != GW_NODE_INVAL) {
            aos_sem_wait(&js_sem, AOS_WAIT_FOREVER);
            if (!is_js_already_existed(g_mgmt_ctx.subdev_ctx->subdevs[i].occ_pid)) {
                gateway_occ_getjs(g_mgmt_ctx.subdev_ctx->subdevs[i].occ_pid);
            } else {
                aos_sem_signal(&js_sem);
            }
        }
    }
}
#endif
#endif

static void _gateway_mesh_get_nodever_timer(void *timer, void *arg)
{
    int      ret            = 0;
    uint16_t total_node_num = ARRAY_SIZE(g_mgmt_ctx.subdev_ctx->subdevs);

    uint16_t init_index = g_mgmt_ctx.get_node_ver_ctx.get_node_ver_idx;

    for (; init_index < total_node_num; init_index++) {
        // if(g_mgmt_ctx.subdev_ctx->subdevs[init_index].state == GW_NODE_ACTIVE) {
        if (g_mgmt_ctx.subdev_ctx->subdevs[init_index].state == GW_NODE_ACTIVE
            && !g_mgmt_ctx.subdev_ctx->subdevs[init_index].version)
        {
            LOGD(TAG, "re-get the version of node %d", g_mgmt_ctx.subdev_ctx->subdevs[init_index].subdev);
            gateway_get_version_msg_t get_version_msg = {
                .subdev = g_mgmt_ctx.subdev_ctx->subdevs[init_index].subdev,
            };
            ret = gateway_mgmt_send_process_msg(GW_SUBDEV_VERSION_GET_EVENT, (void *)&get_version_msg);
            if (ret) {
                LOGE(TAG, "subdev version get process send failed %d", ret);
            }
            init_index++;
            g_mgmt_ctx.get_node_ver_ctx.get_node_ver_idx = init_index;
            break;
        }
    }

    g_mgmt_ctx.get_node_ver_ctx.get_node_ver_idx = init_index;
    if (g_mgmt_ctx.get_node_ver_ctx.get_node_ver_idx >= total_node_num) {
        g_mgmt_ctx.get_node_ver_ctx.get_node_ver_idx = 0;
        g_mgmt_ctx.get_node_ver_ctx.get_node_ver_retry_cnt++;
    }

    if (g_mgmt_ctx.get_node_ver_ctx.get_node_ver_retry_cnt < GET_NODE_VER_CNT_MAX) {
        ret = aos_timer_change_once(&g_mgmt_ctx.get_node_ver_ctx.get_node_ver_timer, GET_NODE_VER_INTERVAL);
        if (ret) {
            LOGE(TAG, "Get node version timer change failed %d", ret);
        }
        ret = aos_timer_start(&g_mgmt_ctx.get_node_ver_ctx.get_node_ver_timer);
        if (ret) {
            LOGE(TAG, "Get node version timer start failed %d", ret);
        }
    } else {
        g_mgmt_ctx.get_node_ver_ctx.get_node_ver_retry_cnt = 0;
    }

    return;
}

gateway_mgmt_callback_t gw_cb = {
    .mgmt_update_event_cb  = _gateway_mgmt_update_event_cb,
    .mgmt_update_status_cb = _gateway_mgmt_update_status_cb,
};

int _gateway_mgmt_init(void)
{
    int             ret                = 0;
    gw_subdev_ctx_t subdev_ctx_from_kv = { 0 };

    aos_task_t mgmt_event_process_task;

    if (g_mgmt_ctx.init_flag) {
        LOGD(TAG, "Init allready");
        return -EALREADY;
    }

    g_mgmt_ctx.subdev_ctx = aos_zalloc(sizeof(gw_subdev_list_t));
    if (g_mgmt_ctx.subdev_ctx == NULL) {
        LOGE(TAG, "alloc g_mgmt_ctx.subdev_ctx fail!!!");
        return -ENOMEM;
    }

    g_mgmt_ctx.subgrp_ctx = aos_zalloc(sizeof(gw_subgrp_list_t));
    if (g_mgmt_ctx.subdev_ctx == NULL) {
        LOGE(TAG, "alloc g_mgmt_ctx.subdev_ctx fail!!!");
        return -ENOMEM;
    }

    for (int i = 1; i < CONFIG_GW_MAX_DEVICES_META_NUM + 1; i++) {
        g_mgmt_ctx.subdev_ctx->subdevs[i].subdev = i;
        g_mgmt_ctx.subdev_ctx->subdevs[i].state  = GW_NODE_INVAL;
    }

    for (int i = 0; i < GW_MAX_GW_GROUP_NUM; i++) {
        g_mgmt_ctx.subgrp_ctx->subgrps[i].subgrp = i;
        g_mgmt_ctx.subgrp_ctx->subgrps[i].state  = GW_SUBGRP_INVAL;
    }

    if ((aos_kv_getint("gw_magic_code", (int *)&(g_mgmt_ctx.subdev_ctx->magic_code)) < 0)
        || (g_mgmt_ctx.subdev_ctx->magic_code != GW_MAGIC_CODE))
    {
        g_mgmt_ctx.subdev_ctx->magic_code = GW_MAGIC_CODE;

        aos_kv_setint("gw_magic_code", (int)g_mgmt_ctx.subdev_ctx->magic_code);
    } else {
        for (int i = 0; i < CONFIG_GW_MAX_DEVICES_META_NUM + 1; i++) {
            subdev_ctx_from_kv.subdev = i;
            ret                       = gateway_subdev_ctx_load(&subdev_ctx_from_kv);
            if (ret == 0) {
                memcpy(&g_mgmt_ctx.subdev_ctx->subdevs[i], &subdev_ctx_from_kv, sizeof(subdev_ctx_from_kv));
                g_mgmt_ctx.subdev_ctx->subdevs[i].hb_last_recv = aos_now_ms();
            }
        }
    }

    g_mgmt_ctx.mgmt_event_queue_buf = aos_zalloc(GW_SUB_DEV_QUEUE_MAX_SIZE);
    if (g_mgmt_ctx.mgmt_event_queue_buf == NULL) {
        LOGE(TAG, "no buf for gateway_add_subdev_queue");
        return -ENOMEM;
    }

    ret = aos_queue_new(&g_mgmt_ctx.msg_queue, g_mgmt_ctx.mgmt_event_queue_buf, GW_SUB_DEV_QUEUE_MAX_SIZE,
                        GW_SUB_DEV_QUEUE_MSG_SIZE);
    if (0 != ret) {
        LOGE(TAG, "aos_queue_new failed");
        return -ENOMEM;
    }

    ret = aos_task_new_ext(&mgmt_event_process_task, "mgmt_event_process_task", _gateway_mgmt_event_process_task, NULL,
                           1024 * 8, AOS_MAX_APP_PRI - 3);
    if (ret != 0) {
        LOGE(TAG, "add_subdev_task create fail");
        return -ENOMEM;
    }

    g_mgmt_ctx.get_node_ver_ctx.get_node_ver_idx = 0;
    ret = aos_timer_new(&g_mgmt_ctx.get_node_ver_ctx.get_node_ver_timer, _gateway_mesh_get_nodever_timer, NULL,
                        GET_NODE_VER_TIMEOUT, 0);
    if (ret != 0) {
        LOGE(TAG, "gateway mgmt offline check enable failed %d", ret);
        return -ENOMEM;
    }

    gateway_mesh_mgmt_cb_register(gw_cb);
    gateway_ota_mgmt_cb_register(g_mgmt_ctx.subdev_status_cb);

    return ret;
}

#if defined(CONFIG_OCC_AUTH) && (CONFIG_OCC_AUTH)
static gw_occ_auth_cb_t gateway_occ_cb = {
    .auth_cb = _gateway_occ_auth_cb,
};
#endif

#if defined(CONFIG_TSL_DEVICER_MESH) && CONFIG_TSL_DEVICER_MESH
int bt_mesh_device_to_cloud_hook(uint32_t opcode, struct net_buf_simple *buf, uint16_t unicast_addr)
{
    int                    rc;
    dev_data_t             dev_data;
    gw_subdev_ctx_t *      subdev_ctx;
    int16_t                subdev_idx;
    struct bt_mesh_node_t *node                 = NULL;
    char                   occ_pid[32]          = { 0 };
    uint8_t                idx[GW_DEV_ADDR_LEN] = { 0x00 };
    dev_conf_t             conf;

    subdev_ctx = _gateway_get_mesh_subdev_ctx(unicast_addr);
    if (!subdev_ctx) {
        return -1;
    }

    dev_data.opcode    = opcode;
    dev_data.data      = (char *)buf->data;
    dev_data.data_size = buf->len;

    rc = gateway_mgmt_get_mesh_subdev_occpid(subdev_ctx->subdev, occ_pid);
    if (rc == -1) {
        LOGE(TAG, "don't have correct occ_pid");
        return -1;
    }

    device_t *dev = dev_new(DEV_TYPE_MESH, occ_pid);

    /* The gateway use subdev idx to identify sub device,
       so store subdev's idx instead of mac address */
    subdev_idx = _gateway_subdev_get_index(subdev_ctx->subdev);
    idx[0]     = subdev_idx & 0xFF;
    idx[1]     = (subdev_idx >> 8) & 0xFF;
    conf.mac   = idx;
    dev_config(dev, (const dev_conf_t *)&conf);

    rc = device_to_cloud(dev, (char *)&dev_data, sizeof(dev_data.data));
    LOGD(TAG, "rc = %d\n", rc);
    dev_free(dev);

    return 0;
}
#endif

/*********************************************************************************************************
 *                                         USER API                                                       *
 *********************************************************************************************************/
gw_status_t gateway_init(void)
{
    gw_status_t ret = 0;

    ret = _gateway_mgmt_init();
    if (ret && ret != -EALREADY) {
        LOGE(TAG, "Gateway mgmt init failed %d", ret);
        return ret;
    }

    gw_ota_cb_t ota_cb     = { 0x00 };
    ota_cb.firmware_use_cb = (gw_subdev_ota_cb_t)g_mgmt_ctx.gateway_event_cb;
    ota_cb.subdev_ota_cb   = (gw_ota_firmware_cb_t)g_mgmt_ctx.subdev_status_cb;
    ret                    = gateway_ota_init(&ota_cb);
    if (ret) {
        LOGE(TAG, "Gateway ota init failed %d", ret);
        return ret;
    }

    ret = gateway_mesh_init();
    if (ret != 0) {
        LOGE(TAG, "gateway mgmt mesh transport init failed %d", ret);
        return ret;
    }

#if defined(CONFIG_GATEWAY_SUBDEV_OFFLINE_CHECK) && CONFIG_GATEWAY_SUBDEV_OFFLINE_CHECK > 0
    ret = gateway_mgmt_offline_check_init();
    if (ret != 0) {
        LOGE(TAG, "gateway mgmt offline check init failed %d", ret);
        return -1;
    }
    ret = gateway_heartbeat_mgmt_enable(1);
    if (ret != 0) {
        LOGE(TAG, "gateway mgmt offline check enable failed %d", ret);
        return -1;
    }
#endif

    g_mgmt_ctx.init_flag = 1;

    return ret;
}

gw_status_t gateway_config(gw_config_e config)
{
    gw_status_t ret = 0;

    g_mgmt_ctx.config = config;

    if (config & GW_CONFIG_SUPPORT_OCC_AUTH) {
#if !defined(CONFIG_OCC_AUTH) || !CONFIG_OCC_AUTH
        LOGE(TAG, "Gateway occ auth not support");
        return -ENOPROTOOPT;
#else
        ret = gateway_occ_auth_init();
        if (ret && ret != -EALREADY) {
            LOGE(TAG, "Gateway occ auth init failed %d", ret);
            return ret;
        }

        ret = gateway_occ_auth_enable(1);
        if (ret) {
            LOGE(TAG, "Gateway occ auth enable failed %d", ret);
            return ret;
        }

        ret = gateway_occ_auth_cb_register(&gateway_occ_cb);
        if (ret) {
            LOGE(TAG, "Gateway occ auth cb register failed %d", ret);
            return ret;
        }
#endif
    } else {
#if defined(CONFIG_OCC_AUTH) && (CONFIG_OCC_AUTH)
        gateway_occ_auth_enable(0);
#endif
    }

    if (config & GW_CONFIG_SUPPORT_TSL_ENGINE) {
#if defined(CONFIG_OCC_AUTH) && (CONFIG_OCC_AUTH)
#if !defined(CONFIG_TSL_DEVICER_MESH) || !CONFIG_TSL_DEVICER_MESH
        LOGE(TAG, "Gateway tsl engine not support");
        return -ENOPROTOOPT;
#else
        ret = gateway_jse_init(_gateway_mgmt_get_occ_js);
        if (ret) {
            LOGE(TAG, "Gatewayjse init failed %d", ret);
            return ret;
        }
#endif
#endif
    }

    return ret;
}

gw_status_t gateway_discover_start(uint32_t timeout)
{
    return gateway_porting_gw_discover(1, timeout);
}

gw_status_t gateway_discover_stop(void)
{
    return gateway_porting_gw_discover(0, 0);
}

gw_status_t gateway_subdev_add(gw_discovered_dev_t subdev)
{
#if defined(CONFIG_OCC_AUTH) && (CONFIG_OCC_AUTH)
    if (_is_config_enabled(GW_CONFIG_SUPPORT_OCC_AUTH)) {

        int           ret          = 0;
        gw_auth_in_t  auth_in_info = { 0x0 };
        gw_auth_out_t auth_out     = { 0x0 };

        switch (subdev.protocol) {
            case GW_NODE_BLE_MESH: {
                memcpy(auth_in_info.in_data.mesh_occ_in_data.addr, subdev.protocol_info.ble_mesh_info.dev_addr, 6);
                memcpy(auth_in_info.in_data.mesh_occ_in_data.uuid, subdev.protocol_info.ble_mesh_info.uuid, 16);
                auth_in_info.dev_protocal                       = GW_OCC_AUTH_TYPE_MESH;
                auth_in_info.in_data.mesh_occ_in_data.addr_type = subdev.protocol_info.ble_mesh_info.addr_type;
                auth_in_info.in_data.mesh_occ_in_data.bearer    = subdev.protocol_info.ble_mesh_info.bearer;
                auth_in_info.in_data.mesh_occ_in_data.oob_info  = subdev.protocol_info.ble_mesh_info.oob_info;
            } break;
            default:
                LOGE(TAG, "Not support auth for protocol %02x", subdev.protocol);
                return -EINVAL;
        }

        ret = gateway_occ_node_auth_result_get(auth_in_info, &auth_out, GW_DEF_OCC_AUTH_NODE_TIMEOUT);
        if (ret) {
            LOGE(TAG, "Get occ auth result failed %d", ret);
            return ret;
        }

        if (auth_out.auth_status == OCC_AUTH_FAILED) {
            LOGE(TAG, "Occ auth failed");
            return -1;
        }
    }
#endif

    return gateway_porting_subdev_add(subdev);
}

gw_status_t gateway_subdev_del(gw_subdev_t subdev)
{
    gw_status_t      ret        = 0;
    gw_subdev_ctx_t *subdev_ctx = NULL;

    subdev_ctx = gateway_subdev_get_ctx(subdev);
    if (!subdev_ctx) {
        LOGE(TAG, "subdev[%d] is invalid", _gateway_subdev_get_index(subdev));
        return -EINVAL;
    }

    ret = gateway_porting_subdev_del(subdev_ctx);
    if (ret) {
        LOGE(TAG, "Del subdev[%d] failed", _gateway_subdev_get_index(subdev));
        return ret;
    }

    /* delete local subdev info */
    memset(subdev_ctx, 0, sizeof(gw_subdev_ctx_t));
    subdev_ctx->state = GW_NODE_INVAL;
    gateway_subdev_ctx_delete(subdev_ctx);
    gateway_subdev_name_delete(subdev_ctx);

    return ret;
}

gw_status_t gateway_subdev_set_name(gw_subdev_t subdev, const char *name)
{
    gw_subdev_ctx_t *        subdev_ctx = NULL;
    gw_evt_subdev_set_name_t gw_evt     = { 0 };

    if (name == NULL) {
        LOGE(TAG, "subdev_set_name: name is null");
        return -EINVAL;
    }

    subdev_ctx = gateway_subdev_get_ctx(subdev);
    if (!subdev_ctx) {
        LOGE(TAG, "subdev[%d] is invalid", subdev);
        return -EINVAL;
    }

    subdev_ctx->name = name;
    gateway_subdev_name_store(subdev_ctx);

    gw_evt.status = 0;
    gw_evt.subdev = subdev;
    if (g_mgmt_ctx.gateway_event_cb) {
        g_mgmt_ctx.gateway_event_cb(GW_SUBDEV_EVT_SET_NAME, (gw_evt_param_t)&gw_evt);
    }

    return 0;
}

gw_status_t gateway_subgrp_create(const char *name)
{
    gw_status_t      ret        = -1;
    uint8_t          i          = 0;
    gw_subgrp_ctx_t *subgrp_ctx = NULL;

    gw_evt_subgrp_create_t gw_evt = { 0 };

    if (name == NULL) {
        LOGE(TAG, "subgrp_create: name is null");
        return -EINVAL;
    }

    for (; i < GW_MAX_GW_GROUP_NUM; i++) {
        subgrp_ctx = &g_mgmt_ctx.subgrp_ctx->subgrps[i];
        if (subgrp_ctx->state == GW_SUBGRP_INVAL) {
            subgrp_ctx->state  = GW_SUBGRP_VALID;
            subgrp_ctx->name   = name;
            subgrp_ctx->subgrp = i;

            ret = gateway_subgrp_ctx_store(subgrp_ctx);
            ret |= gateway_subgrp_name_store(subgrp_ctx);

            break;
        }
    }

    gw_evt.status = ret;
    gw_evt.subgrp = subgrp_ctx->subgrp;
    if (g_mgmt_ctx.gateway_event_cb) {
        g_mgmt_ctx.gateway_event_cb(GW_SUBGRP_EVT_CREATE, (gw_evt_param_t)&gw_evt);
    }

    return ret;
}

gw_status_t gateway_subgrp_del(gw_subgrp_t subgrp)
{
    gw_status_t      ret        = 0;
    gw_subgrp_ctx_t *subgrp_ctx = NULL;

    gw_evt_subgrp_del_t gw_evt = { 0 };

    subgrp_ctx = _gateway_subgrp_get_ctx(subgrp);
    if (!subgrp_ctx) {
        LOGW(TAG, "the subgrp[%d] has been deleted", subgrp);
        return -EINVAL;
    }

    memset(subgrp_ctx, 0, sizeof(gw_subgrp_ctx_t));
    subgrp_ctx->state = GW_SUBGRP_INVAL;

    ret = gateway_subgrp_ctx_delete(subgrp_ctx);
    ret |= gateway_subgrp_name_delete(subgrp_ctx);
    if (ret != 0) {
        LOGE(TAG, "del subgrp name fail");
        goto exit;
    }

exit:
    gw_evt.status = ret;
    gw_evt.subgrp = subgrp_ctx->subgrp;
    if (g_mgmt_ctx.gateway_event_cb) {
        g_mgmt_ctx.gateway_event_cb(GW_SUBGRP_EVT_DEL, (gw_evt_param_t)&gw_evt);
    }

    return ret;
}

gw_status_t gateway_subgrp_set_name(gw_subgrp_t subgrp, const char *name)
{
    gw_status_t      ret        = -1;
    gw_subgrp_ctx_t *subgrp_ctx = NULL;

    gw_evt_subgrp_set_name_t gw_evt = { 0 };

    if (name == NULL) {
        LOGE(TAG, "subgrp set name: name is null");
        return -EINVAL;
    }

    subgrp_ctx = _gateway_subgrp_get_ctx(subgrp);
    if (!subgrp_ctx) {
        LOGE(TAG, "the subgrp[%d] is invalid", subgrp);
        return -EINVAL;
    }

    subgrp_ctx->name = name;
    ret              = gateway_subgrp_name_store(subgrp_ctx);

    gw_evt.status = ret;
    gw_evt.subgrp = subgrp_ctx->subgrp;
    if (g_mgmt_ctx.gateway_event_cb) {
        g_mgmt_ctx.gateway_event_cb(GW_SUBGRP_EVT_SET_NAME, (gw_evt_param_t)&gw_evt);
    }

    return ret;
}

gw_status_t gateway_subgrp_add_subdev(gw_subgrp_t subgrp, gw_subdev_t subdev)
{
    gw_subdev_ctx_t *subdev_ctx = NULL;
    gw_subgrp_ctx_t *subgrp_ctx = NULL;

    subgrp_ctx = _gateway_subgrp_get_ctx(subgrp);
    if (!subgrp_ctx) {
        LOGE(TAG, "the subgrp[%d] is invalid", subgrp);
        return -EINVAL;
    }

    subdev_ctx = gateway_subdev_get_ctx(subdev);
    if (!subdev_ctx) {
        LOGE(TAG, "subdev[%d] is invalid", subdev);
        return -EINVAL;
    }

    return gateway_porting_subdev_sub_add(subdev_ctx, subgrp);
}

gw_status_t gateway_subgrp_add_subdev_list(gw_subgrp_t subgrp, gw_subdev_t *subdev_list, uint8_t subdev_num)
{
    gw_status_t      ret        = -1;
    gw_subdev_ctx_t *subdev_ctx = NULL;
    gw_subgrp_ctx_t *subgrp_ctx = NULL;

    if ((subdev_list == NULL) || (subdev_num <= 0)) {
        LOGE(TAG, "input param error");
        return -1;
    }

    subgrp_ctx = _gateway_subgrp_get_ctx(subgrp);
    if (!subgrp_ctx) {
        LOGE(TAG, "the subgrp[%d] is invalid", subgrp);
        return -1;
    }

    for (int i = 0; i < subdev_num; i++) {
        subdev_ctx = gateway_subdev_get_ctx(subdev_list[i]);
        if (!subdev_ctx) {
            LOGE(TAG, "the subdev[%d] is invalid", subdev_list[i]);
            return -EINVAL;
        }
        ret = gateway_porting_subdev_sub_add(subdev_ctx, subgrp);
        if (ret) {
            return ret;
        }
    }

    return 0;
}

gw_status_t gateway_subgrp_del_subdev(gw_subgrp_t subgrp, gw_subdev_t subdev)
{
    gw_subdev_ctx_t *subdev_ctx = NULL;
    gw_subgrp_ctx_t *subgrp_ctx = NULL;

    subgrp_ctx = _gateway_subgrp_get_ctx(subgrp);
    if (subgrp_ctx->state == GW_SUBGRP_INVAL) {
        LOGE(TAG, "the subgrp[%d] is invalid", subgrp);
        return -EINVAL;
    }

    subdev_ctx = gateway_subdev_get_ctx(subdev);
    if (!subdev_ctx) {
        LOGE(TAG, "the subdev[%d] is invalid", subdev);
        return -EINVAL;
    }

    return gateway_porting_subdev_sub_del(subdev_ctx, subgrp);
}

gw_status_t gateway_subgrp_del_subdev_list(gw_subgrp_t subgrp, gw_subdev_t *subdev_list, uint8_t subdev_num)
{
    gw_status_t      ret        = -1;
    gw_subdev_ctx_t *subdev_ctx = NULL;
    gw_subgrp_ctx_t *subgrp_ctx = NULL;

    if ((subdev_list == NULL) || (subdev_num <= 0)) {
        LOGE(TAG, "input param error");
        return -EINVAL;
    }

    subgrp_ctx = _gateway_subgrp_get_ctx(subgrp);
    if (subgrp_ctx->state == GW_SUBGRP_INVAL) {
        LOGE(TAG, "the subgrp[%d] is invalid", subgrp);
        return -EINVAL;
    }

    for (int i = 0; i < subdev_num; i++) {
        subdev_ctx = gateway_subdev_get_ctx(subdev_list[i]);
        if (subdev_ctx->state == GW_NODE_INVAL) {
            LOGE(TAG, "the subdev[%d] is invalid", subdev_list[i]);
            continue;
        }

        ret = gateway_porting_subdev_sub_del(subdev_ctx, subgrp);
        if (ret) {
            return ret;
        }
    }

    return ret;
}

gw_status_t gateway_subgrp_subdev_foreach(gw_subgrp_t subgrp,
                                          uint8_t (*func)(gw_subgrp_t subgrp, gw_subdev_t subdev, void *data),
                                          void *data)
{
    gw_status_t      ret        = -1;
    gw_subdev_ctx_t *subdev_ctx = NULL;
    gw_subgrp_ctx_t *subgrp_ctx = NULL;

    if (func == NULL) {
        LOGE(TAG, "input param err");
        return -EINVAL;
    }

    subgrp_ctx = _gateway_subgrp_get_ctx(subgrp);
    if (!subgrp_ctx) {
        LOGE(TAG, "the subgrp[%d] is invalid", subgrp);
        return -EINVAL;
    }

    for (int i = 0; i < CONFIG_GW_MAX_DEVICES_META_NUM; i++) {
        subdev_ctx = &g_mgmt_ctx.subdev_ctx->subdevs[i];
        if (subdev_ctx->state == GW_NODE_INVAL) {
            continue;
        }

        for (int j = 0; j < GW_MAX_SUBDEV_JOINED_GRP_NUM; j++) {
            if (subgrp == subdev_ctx->subgrp[j]) {
                ret = func(subgrp, subdev_ctx->subdev, data);
                if (ret == GW_SUBDEV_ITER_STOP) {
                    break;
                }
            }
        }
    }

    return 0;
}

gw_status_t gateway_subgrp_set_onoff(gw_subgrp_t subgrp, uint8_t onoff)

{

  return gateway_porting_subgrp_set_onoff(GW_NODE_BLE_MESH, subgrp, onoff);

}

gw_status_t gateway_subgrp_set_brightness(gw_subgrp_t subgrp, uint16_t brightness)

{

  return gateway_porting_subgrp_set_brightness(GW_NODE_BLE_MESH, subgrp, brightness);

}

gw_status_t gateway_subdev_set_onoff(gw_subdev_t subdev, uint8_t onoff)
{
    gw_subdev_ctx_t *subdev_ctx = NULL;

    subdev_ctx = gateway_subdev_get_ctx(subdev);
    if (!subdev_ctx) {
        LOGE(TAG, "subdev[%d] is invalid", subdev);
        return -EINVAL;
    }

    return gateway_porting_subdev_set_onoff(subdev_ctx, onoff);
}

gw_status_t gateway_subdev_set_brightness(gw_subdev_t subdev, uint16_t brightness)
{
    gw_subdev_ctx_t *subdev_ctx = NULL;

    subdev_ctx = gateway_subdev_get_ctx(subdev);
    if (!subdev_ctx) {
        LOGE(TAG, "subdev[%d] is invalid", subdev);
        return -EINVAL;
    }

    return gateway_porting_subdev_set_brightness(subdev_ctx, brightness);
}

gw_status_t gateway_subdev_send_rawdata(gw_subdev_t subdev, uint8_t *raw_data, int data_len)
{
    gw_subdev_ctx_t *subdev_ctx = NULL;

    if ((raw_data == NULL) || (data_len <= 0)) {
        LOGE(TAG, "send rawdata: input param err");
        return -EINVAL;
    }

    subdev_ctx = gateway_subdev_get_ctx(subdev);
    if (!subdev_ctx) {
        LOGE(TAG, "subdev[%d] is invalid", subdev);
        return -EINVAL;
    }

    return gateway_porting_subdev_send_rawdata(subdev_ctx, raw_data, data_len);
}

gw_status_t gateway_subdev_get_onoff(gw_subdev_t subdev)
{
    gw_subdev_ctx_t *subdev_ctx = NULL;

    subdev_ctx = gateway_subdev_get_ctx(subdev);
    if (!subdev_ctx) {
        LOGE(TAG, "subdev[%d] is invalid", subdev);
        return -EINVAL;
    }

    return gateway_porting_subdev_get_onoff(subdev_ctx);
}

gw_status_t gateway_subdev_get_brightness(gw_subdev_t subdev)
{
    gw_subdev_ctx_t *subdev_ctx = NULL;

    subdev_ctx = gateway_subdev_get_ctx(subdev);
    if (!subdev_ctx) {
        LOGE(TAG, "subdev[%d] is invalid", subdev);
        return -EINVAL;
    }
    return gateway_porting_subdev_get_brightness(subdev_ctx);
}

gw_status_t gateway_subdev_foreach(uint8_t (*func)(gw_subdev_t subdev, void *data), void *data)
{
    int              ret        = 0;
    gw_subdev_ctx_t *subdev_ctx = NULL;

    if (func == NULL) {
        LOGE(TAG, "input param err");
        return -EINVAL;
    }

    for (int i = 1; i < CONFIG_GW_MAX_DEVICES_META_NUM; i++) {
        subdev_ctx = &g_mgmt_ctx.subdev_ctx->subdevs[i];
        if (subdev_ctx->state == GW_NODE_INVAL)
            continue;
        ret = func(subdev_ctx->subdev, data);
        if (ret == GW_SUBDEV_ITER_STOP) {
            break;
        }
    }
    return 0;
}

gw_status_t gateway_subdev_get_info(gw_subdev_t subdev, subdev_info_t *info)
{
    gw_status_t      ret        = -1;
    gw_subdev_ctx_t *subdev_ctx = NULL;

    if (info == NULL) {
        LOGE(TAG, "get info: input param err");
        return -EINVAL;
    }

    subdev_ctx = gateway_subdev_get_ctx(subdev);
    if (!subdev_ctx) {
        LOGE(TAG, "subdev[%d] is invalid", subdev);
        return -EINVAL;
    }

    memset(info, 0, sizeof(subdev_info_t));

    gateway_subdev_name_load(subdev_ctx);

    ret = _gateway_mgmt_subdev_get_mac(subdev_ctx, (char *)info->dev_addr);
    if (ret) {
        LOGW(TAG, "No mac get for subdev[%d]", _gateway_subdev_get_index(subdev));
    }

    info->version = subdev_ctx->version;
    if (subdev_ctx->state == GW_NODE_ACTIVE || subdev_ctx->state == GW_NODE_ACTIVE_CHECK) {
        info->active_status = GW_SUBDEV_ACTIVE;
    } else {
        info->active_status = GW_SUBDEV_NOT_ACTIVE;
    }

#if defined(CONFIG_OCC_AUTH) && (CONFIG_OCC_AUTH)
    if (strlen(subdev_ctx->cid)) {
        strncpy((char *)info->occ_cid, subdev_ctx->cid, strlen(subdev_ctx->cid));
    } else {
        LOGW(TAG, "No CID for subdev[%d]", _gateway_subdev_get_index(subdev));
    }
#endif

    return 0;
}

gw_status_t gateway_subdev_get_cloud_info(gw_subdev_t subdev)
{
    gw_status_t      ret        = -1;
    gw_subdev_ctx_t *subdev_ctx = NULL;

    subdev_ctx = gateway_subdev_get_ctx(subdev);
    if (!subdev_ctx) {
        LOGE(TAG, "subdev[%d] is invalid", subdev);
        return -EINVAL;
    }

    LOGD(TAG, "Get triples local");
    ret = gateway_porting_subdev_get_triples(subdev_ctx, TRIPLE_TYPE_PK | TRIPLE_TYPE_PS);
    if (ret) {
        LOGE(TAG, "Get local triple failed %d", ret);
        return ret;
    }
    return 0;
}

gw_status_t gateway_subdev_model_ctrl(gw_subdev_t subdev, void *model, int model_len)
{
    gw_status_t      ret        = -1;
    gw_subdev_ctx_t *subdev_ctx = NULL;

    if (model == NULL || model_len <= 0) {
        LOGE(TAG, "model ctrl: model param is invaild");
        return -EINVAL;
    }

    subdev_ctx = gateway_subdev_get_ctx(subdev);
    if (!subdev_ctx) {
        LOGE(TAG, "subdev[%d] is invalid", subdev);
        return -EINVAL;
    }

#if defined(CONFIG_OCC_AUTH) && defined(CONFIG_TSL_DEVICER_MESH) && CONFIG_TSL_DEVICER_MESH
    dev_conf_t  conf;
    int16_t     subdev_idx           = -1;
    uint8_t     idx[GW_DEV_ADDR_LEN] = { 0x00 };
    const char *cloud_data           = model;

    /* The gateway use subdev idx to identify sub device,
       so store subdev's idx instead of mac address */
    subdev_idx = _gateway_subdev_get_index(subdev_ctx->subdev);
    idx[0]     = subdev_idx & 0xFF;
    idx[1]     = (subdev_idx >> 8) & 0xFF;
    conf.mac   = idx;

    device_t *dev = dev_new(DEV_TYPE_MESH, subdev_ctx->occ_pid);
    dev_config(dev, (const dev_conf_t *)&conf);
    ret = cloud_to_device(dev, cloud_data, strlen(cloud_data));
    dev_free(dev);
#endif

    return ret;
}

gw_status_t gateway_event_cb_register(gw_event_cb_t cb)
{
    g_mgmt_ctx.gateway_event_cb = cb;

    return 0;
}

gw_status_t gateway_subdev_status_cb_register(gw_subdev_status_cb_t cb)
{
    g_mgmt_ctx.subdev_status_cb = cb;

    return 0;
}
