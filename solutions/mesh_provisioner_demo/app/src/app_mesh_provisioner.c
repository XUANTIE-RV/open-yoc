/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <aos/aos.h>
#include <aos/ble.h>
#include <gateway.h>
#include "common/log.h"

#define TAG      "MESH_PROVISIONER"

gw_evt_discovered_info_t subdev;

static inline char *_state_str(uint8_t state)
{
    switch (state) {
        case GW_SUBDEV_NOT_ACTIVE:
            return "INACTIVE";
        case GW_SUBDEV_ACTIVE:
            return "ACTIVE";
        default:
            return "UNKNOW";
    }
}

static inline char *_reason_str(uint8_t state)
{
    switch (state) {
        case GW_SUBDEV_STATUS_CHANGE_BY_NODE_ADD:
            return "NODE_ADD";
        case GW_SUBDEV_STATUS_CHANGE_BY_NODE_RST:
            return "NODE_RST";
        case GW_SUBDEV_STATUS_CHANGE_BY_ACTIVE_CHECK_FAIL:
            return "ACTIVE_CHECK_FAIL";
        case GW_SUBDEV_STATUS_CHANGE_BY_ACTIVE_CHECK_SUCCESS:
            return "ACTIVE_CHECK_SUCCESS";
        case GW_SUBDEV_STATUS_CHANGE_BY_HB_NOT_ENABLE:
            return "HB_NOT_ENABLE";
        case GW_SUBDEV_STATUS_CHANGE_BY_HB_ERR_PARAM:
            return "HB_ERR_PARAM";
        case GW_SUBDEV_STATUS_CHANGE_BY_HB_RECV:
            return "HB_RECV";
        case GW_SUBDEV_STATUS_CHANGE_BY_VERSION_RECV:
            return "VERSION_RECV";
        case GW_SUBDEV_STATUS_CHANGE_BY_HB_LOST:
            return "HB_LOST";
        default:
            return "UNKNOW";
    }
}

/* 配网器配网事件回调处理函数 */
gw_status_t appmesh_provisioner_cb(gw_event_type_e gw_evt, gw_evt_param_t gw_evt_param)
{
    LOGE(TAG, "Gw event:%02x", gw_evt);
    switch (gw_evt)
    {

    case GW_EVT_FOUND_DEV:
    {
        gw_evt_discovered_info_t *scan_msg = (gw_evt_discovered_info_t *)gw_evt_param;

        if (scan_msg->protocol == GW_NODE_BLE_MESH) {
                LOGI(TAG, "Found Mesh Node:%s,%s", bt_hex_real(scan_msg->protocol_info.ble_mesh_info.dev_addr, 6),
                     bt_hex_real(scan_msg->protocol_info.ble_mesh_info.uuid, 16));
            }
        memcpy(&subdev, scan_msg, sizeof(gw_discovered_dev_t));
    } break;
    case GW_EVT_FOUND_DEV_TIMEOUT: {
            LOGE(TAG, "scan dev timeout");
    } break;
    case GW_SUBDEV_EVT_ADD_FAIL: {
        LOGE(TAG, "add provisioner failed");
    } break;
    default:
    break;
    }
    return 0;
}

gw_status_t appmesh_provisioner_model_cb(gw_subdev_t subdev, gw_subdev_status_type_e gw_status,
                                         gw_status_param_t status, int status_len)
{
    LOGE(TAG, "Subdev[%d] status:%02x", subdev, gw_status);
    switch (gw_status) {
        case GW_SUBDEV_STATUS_HB_SET: {
            gw_subdev_hb_set_status_t *hb_set_status = (gw_subdev_hb_set_status_t *)status;
            LOGE(TAG, "Subdev %d Hb set status:%02x,period:%04x", subdev, hb_set_status->status,
                 hb_set_status->period_s);
            (void)hb_set_status;
        } break;
        case GW_SUBDEV_STATUS_CHANGE: {
            gw_status_subdev_t *subdev_status = (gw_status_subdev_t *)status;
            LOGE(TAG, "Subdev %d status change from %s ---> %s, reason:%s", subdev,
                 _state_str(subdev_status->old_status), _state_str(subdev_status->new_status),
                 _reason_str(subdev_status->reason));
            (void)subdev_status;
        } break;
        default:
            break;
    }

    return 0;
}

void appmesh_provisioner_init(void)
{
    gateway_event_cb_register(appmesh_provisioner_cb);
    gateway_subdev_status_cb_register(appmesh_provisioner_model_cb);
    gateway_init();

    return;
}