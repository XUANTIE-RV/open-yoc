/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include "mesh_model/mesh_model.h"
#include "port/mesh_event_port.h"
#ifdef CONFIG_BT_MESH_PROVISIONER
#include "provisioner_main.h"
#endif

#define TAG "MESH_EVENT_PORT"

void mesh_model_event_cb(mesh_model_event_e event, void *p_arg)
{
    mesh_model_event_en mesh_model_evt;

    model_message message;

    switch (event) {
        case BT_MESH_MODEL_EVT_APPKEY_ADD: {
            mesh_model_evt = BT_MESH_MODEL_CFG_APP_KEY_ADD;
            model_event(mesh_model_evt, p_arg);
        } break;

        case BT_MESH_MODEL_EVT_COMP_DATA_STATUS: {
            mesh_model_evt                = BT_MESH_MODEL_CFG_COMP_DATA_STATUS;
            bt_mesh_model_evt_t *evt_data = (bt_mesh_model_evt_t *)p_arg;

            if (evt_data) {
                message.source_addr = evt_data->source_addr;
                message.status_data = evt_data->user_data;
            }

            model_event(mesh_model_evt, &message);
        } break;

        case BT_MESH_MODEL_EVT_BEACON_STATUS: {
            mesh_model_evt                = BT_MESH_MODEL_CFG_BEACON_STATUS;
            bt_mesh_model_evt_t *evt_data = (bt_mesh_model_evt_t *)p_arg;

            if (evt_data) {
                message.source_addr = evt_data->source_addr;
                message.status_data = evt_data->user_data;
            }

            model_event(mesh_model_evt, &message);
        } break;

        case BT_MESH_MODEL_EVT_TTL_STATUS: {
            mesh_model_evt                = BT_MESH_MODEL_CFG_TTL_STATUS;
            bt_mesh_model_evt_t *evt_data = (bt_mesh_model_evt_t *)p_arg;

            if (evt_data) {
                message.source_addr = evt_data->source_addr;
                message.status_data = evt_data->user_data;
            }

            model_event(mesh_model_evt, &message);
        } break;

        case BT_MESH_MODEL_EVT_FRIEND_STATUS: {
            mesh_model_evt                = BT_MESH_MODEL_CFG_FRIEND_STATUS;
            bt_mesh_model_evt_t *evt_data = (bt_mesh_model_evt_t *)p_arg;

            if (evt_data) {
                message.source_addr = evt_data->source_addr;
                message.status_data = evt_data->user_data;
            }

            model_event(mesh_model_evt, &message);
        } break;

        case BT_MESH_MODEL_EVT_PROXY_STATUS: {
            mesh_model_evt                = BT_MESH_MODEL_CFG_PROXY_STATUS;
            bt_mesh_model_evt_t *evt_data = (bt_mesh_model_evt_t *)p_arg;

            if (evt_data) {
                message.source_addr = evt_data->source_addr;
                message.status_data = evt_data->user_data;
            }

            model_event(mesh_model_evt, &message);
        } break;

        case BT_MESH_MODEL_EVT_RELAY_STATUS: {
            mesh_model_evt                = BT_MESH_MODEL_CFG_RELAY_STATUS;
            bt_mesh_model_evt_t *evt_data = (bt_mesh_model_evt_t *)p_arg;

            if (evt_data) {
                message.source_addr = evt_data->source_addr;
                message.status_data = evt_data->user_data;
            }

            model_event(mesh_model_evt, &message);
        } break;

        case BT_MESH_MODEL_EVT_NETKEY_STATUS: {
            mesh_model_evt                = BT_MESH_MODEL_CFG_NET_KEY_STATUS;
            bt_mesh_model_evt_t *evt_data = (bt_mesh_model_evt_t *)p_arg;

            if (evt_data) {
                message.source_addr = evt_data->source_addr;
                message.status_data = evt_data->user_data;
            }

            model_event(mesh_model_evt, &message);
        } break;

        case BT_MESH_MODEL_EVT_NET_KRP_STATUS: {
            mesh_model_evt                = BT_MESH_MODEL_CFG_NET_KRP_STATUS;
            bt_mesh_model_evt_t *evt_data = (bt_mesh_model_evt_t *)p_arg;

            if (evt_data) {
                message.source_addr = evt_data->source_addr;
                message.status_data = evt_data->user_data;
            }

            model_event(mesh_model_evt, &message);
        } break;

        case BT_MESH_MODEL_EVT_APPKEY_STATUS: {
            mesh_model_evt                = BT_MESH_MODEL_CFG_APPKEY_STATUS;
            bt_mesh_model_evt_t *evt_data = (bt_mesh_model_evt_t *)p_arg;

            if (evt_data) {
                message.source_addr = evt_data->source_addr;
                message.status_data = evt_data->user_data;
            }

            model_event(mesh_model_evt, &message);
        } break;

        case BT_MESH_MODEL_EVT_APPKEY_BIND_STATUS: {
            mesh_model_evt                = BT_MESH_MODEL_CFG_APPKEY_BIND_STATUS;
            bt_mesh_model_evt_t *evt_data = (bt_mesh_model_evt_t *)p_arg;

            if (evt_data) {
                message.source_addr = evt_data->source_addr;
                message.status_data = evt_data->user_data;
            }

            model_event(mesh_model_evt, &message);
        } break;

        case BT_MESH_MODEL_EVT_PUB_STATUS: {
            mesh_model_evt                = BT_MESH_MODEL_CFG_PUB_STATUS;
            bt_mesh_model_evt_t *evt_data = (bt_mesh_model_evt_t *)p_arg;

            if (evt_data) {
                message.source_addr = evt_data->source_addr;
                message.status_data = evt_data->user_data;
            }

            model_event(mesh_model_evt, &message);
        } break;

        case BT_MESH_MODEL_EVT_SUB_STATUS: {
            mesh_model_evt                = BT_MESH_MODEL_CFG_SUB_STATUS;
            bt_mesh_model_evt_t *evt_data = (bt_mesh_model_evt_t *)p_arg;

            if (evt_data) {
                message.source_addr = evt_data->source_addr;
                message.status_data = evt_data->user_data;
            }

            model_event(mesh_model_evt, &message);
        } break;

        case BT_MESH_MODEL_EVT_SUB_LIST: {
            mesh_model_evt                = BT_MESH_MODEL_CFG_SUB_LIST;
            bt_mesh_model_evt_t *evt_data = (bt_mesh_model_evt_t *)p_arg;

            if (evt_data) {
                message.source_addr = evt_data->source_addr;
                message.status_data = evt_data->user_data;
            }

            model_event(mesh_model_evt, &message);
        } break;

        case BT_MESH_MODEL_EVT_SUB_LIST_VND: {
            mesh_model_evt                = BT_MESH_MODEL_CFG_SUB_LIST_VND;
            bt_mesh_model_evt_t *evt_data = (bt_mesh_model_evt_t *)p_arg;

            if (evt_data) {
                message.source_addr = evt_data->source_addr;
                message.status_data = evt_data->user_data;
            }

            model_event(mesh_model_evt, &message);
        } break;

        case BT_MESH_MODEL_EVT_HB_SUB_STATUS: {
            mesh_model_evt                = BT_MESH_MODEL_CFG_HEARTBEAT_SUB_STATUS;
            bt_mesh_model_evt_t *evt_data = (bt_mesh_model_evt_t *)p_arg;

            if (evt_data) {
                message.source_addr = evt_data->source_addr;
                message.status_data = evt_data->user_data;
            }

            model_event(mesh_model_evt, &message);
        } break;

        case BT_MESH_MODEL_EVT_HB_PUB_STATUS: {
            mesh_model_evt                = BT_MESH_MODEL_CFG_HEARTBEAT_PUB_STATUS;
            bt_mesh_model_evt_t *evt_data = (bt_mesh_model_evt_t *)p_arg;
#ifdef CONFIG_BT_MESH_PROVISIONER
            struct net_buf_simple_state state;
            struct net_buf_simple *     hb_status_data = (struct net_buf_simple *)evt_data->user_data;
            net_buf_simple_save(hb_status_data, &state);
            uint8_t  status     = net_buf_simple_pull_u8(hb_status_data);
            uint16_t pub_dst    = net_buf_simple_pull_le16(hb_status_data);
            uint8_t  pub_count  = net_buf_simple_pull_u8(hb_status_data);
            uint8_t  pub_period = net_buf_simple_pull_u8(hb_status_data);
            (void)pub_count;
            (void)pub_dst;
            (void)status;
            net_buf_simple_restore(hb_status_data, &state);
            struct bt_mesh_node_t *node = bt_mesh_provisioner_get_node_info(evt_data->source_addr);
            if (!node) {
                return;
            } else {
                extern int provisioner_node_hb_log_set(int node_index, u32_t hb_period_log);
                provisioner_node_hb_log_set(bt_mesh_provisioner_get_node_id(node), pub_period);
            }
#endif

            if (evt_data) {
                message.source_addr = evt_data->source_addr;
                message.status_data = evt_data->user_data;
            }

            model_event(mesh_model_evt, &message);
        } break;
        case BT_MESH_MODEL_EVT_HEALTH_FAULTS_STATUS: {
            mesh_model_evt                = BT_MESH_MODEL_HEALTH_FAULT_STATUS;
            bt_mesh_model_evt_t *evt_data = (bt_mesh_model_evt_t *)p_arg;

            if (evt_data) {
                message.source_addr = evt_data->source_addr;
                message.status_data = evt_data->user_data;
            }

            model_event(mesh_model_evt, &message);
        } break;
        case BT_MESH_MODEL_EVT_HEARTBEAT_STATUS: {
            mesh_model_evt = BT_MESH_MODEL_CFG_HEARTBEAT_STATUS;
            model_event(mesh_model_evt, p_arg);
        } break;

        case BT_MESH_MODEL_EVT_NODE_RESET_STATUS: {

            mesh_model_evt                = BT_MESH_MODEL_CFG_RST_STATUS;
            bt_mesh_model_evt_t *evt_data = (bt_mesh_model_evt_t *)p_arg;

            if (evt_data) {
                message.source_addr = evt_data->source_addr;
                message.status_data = evt_data->user_data;
#ifdef CONFIG_BT_MESH_PROVISIONER
                if (!bt_mesh_provisioner_get_node_info(evt_data->source_addr)) {
                    return;
                }
#endif
                model_event(mesh_model_evt, &message);
            }

        } break;

        /*[Genie begin] add by wenbing.cwb at 2021-01-21*/
#ifdef CONFIG_BT_MESH_CTRL_RELAY
        case BT_MESH_MODEL_EVT_CTRL_RELAY_STATUS: {
            mesh_model_evt                = BT_MESH_MODEL_CFG_CTRL_RELAY_STATUS;
            bt_mesh_model_evt_t *evt_data = (bt_mesh_model_evt_t *)p_arg;

            if (evt_data) {
                message.source_addr = evt_data->source_addr;
                message.status_data = evt_data->user_data;
            }

            model_event(mesh_model_evt, &message);
        } break;
#endif
            /*[Genie end] add by wenbing.cwb at 2021-01-21*/

        case BT_MESH_MODEL_EVT_RPL_IS_FULL: {
            // rpl is full,do something
        } break;

        default:
            return;
    }

    return;
}

int mesh_event_port_init(void)
{
    bt_mesh_event_register(mesh_model_event_cb);
    return 0;
}
