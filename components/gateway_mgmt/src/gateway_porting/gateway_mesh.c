/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <aos/aos.h>

#include "common/log.h"
#include "api/mesh.h"
#include "mesh_model/mesh_model.h"
#include "mesh_node.h"
#include "mesh_provisioner.h"
#include "provisioner_main.h"
#include "inc/net.h"
#include "inc/access.h"
#include "gateway_mesh.h"
#include "gateway_mgmt.h"
#include "gateway.h"

#include "at_mesh.h"

#define TAG                           "GATEWAY_MESH"
#define BT_MESH_ADDR_IS_UNICAST(addr) ((addr) && (addr) < 0x8000)
#define BIT_MASK(n)                   (BIT(n) - 1)
#define ERR_CHAR                      0XFF

typedef struct {
    uint8_t type;
    uint8_t val[6];
} device_mac_t;

static set_onoff_arg           set_onoff_data     = { 0 };
static set_lightness_arg       set_lightness_data = { 0 };
static set_light_ctl_arg       set_light_ctl_data = { 0 };
static gateway_mgmt_callback_t g_gateway_mgmt_callback;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-braces" /* ignore the missing braces warnings here */

static struct bt_mesh_model elem0_root_models[] = {
    MESH_MODEL_CFG_SRV_NULL(),       MESH_MODEL_CFG_CLI_NULL(),       MESH_MODEL_HEALTH_SRV_NULL(),
    MESH_MODEL_HEALTH_CLI_NULL(),    MESH_MODEL_GEN_ONOFF_CLI_NULL(), MESH_MODEL_GEN_LEVEL_CLI_NULL(),
    MESH_MODEL_LIGHTNESS_CLI_NULL(), MESH_MODEL_LIGHT_CTL_CLI_NULL(), MESH_MODEL_GEN_ONOFF_SRV_NULL(),
    MESH_MODEL_BLOB_CLI_NULL(),
};
static struct bt_mesh_model elem0_vnd_models[] = {
    MESH_MODEL_VENDOR_CLI_NULL(),
};
static struct bt_mesh_elem elements[] = {
    BT_MESH_ELEM(0, elem0_root_models, elem0_vnd_models, 0),
};
/*comp*/
static const struct bt_mesh_comp mesh_comp = {
    .cid        = 0x01A8,
    .elem       = elements,
    .elem_count = ARRAY_SIZE(elements),
};

provisioner_node_config_t g_provisioner_param = {
    .unicast_start_addr = CONFIG_GW_MESH_GW_START_ADDR,
    .unicast_end_addr   = CONFIG_GW_MESH_GW_END_ADDR,
    .attention_time     = DEF_ATTENTION_TIMEOUT,
};

#pragma GCC diagnostic pop /* end of ignore missing braces warnings */

static u16_t hb_pwr2(u8_t val, u8_t sub)
{
    if (!val) {
        return 0x0000;
    } else if (val == 0xff || val == 0x11) {
        return 0xffff;
    } else {
        return (1 << (val - sub));
    }
}

static uint8_t hb_get_log(u16_t val, u8_t sub)
{
    if (val == 0xFFFF) {
        return 0xFF;
    } else {
        uint8_t log = 0;
        while (val > 1) {
            val = val >> 1;
            log++;
        }
        return log + sub;
    }
}

static inline int bt_addr_val_to_str(const uint8_t addr[6], char *str, size_t len)
{
    return snprintf(str, len, "%02X:%02X:%02X:%02X:%02X:%02X", addr[5], addr[4], addr[3], addr[2], addr[1], addr[0]);
}

int gateway_mesh_auth_input(uint8_t action, void *data, u8_t size)
{
    if (action == GW_AUTH_INPUT_NUM) {
        return ble_mesh_provisioner_OOB_input_num(*(uint32_t *)data);
    } else if (action == GW_AUTH_INPUT_STR) {
        return ble_mesh_provisioner_OOB_input_string(data);
    } else {
        LOGE(TAG, "Unsupport auth method");
    }
    return -1;
}

static void gateway_mesh_provisioner_cb(mesh_provisioner_event_en event, void *p_arg)
{
    switch (event) {
        // provisioner message
        case BT_MESH_EVENT_RECV_UNPROV_DEV_ADV: {
            if (p_arg) {
                mesh_node_t *            node = (mesh_node_t *)p_arg;
                uint8_t                  dev_addr_str[20];
                uint8_t                  uuid_str[40];
                gw_evt_discovered_info_t scan_msg = { 0 };
                bt_addr_val_to_str(node->dev_addr, (char *)dev_addr_str, sizeof(dev_addr_str));
                LOGD(TAG, "Found mesh dev:%s,addr_type:%02x,oob_info:%02x,beraer:%02x", dev_addr_str, node->addr_type,
                     node->oob_info, node->bearer);
                sprintf((char *)uuid_str, "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
                        node->uuid[0], node->uuid[1], node->uuid[2], node->uuid[3], node->uuid[4], node->uuid[5],
                        node->uuid[6], node->uuid[7], node->uuid[8], node->uuid[9], node->uuid[10], node->uuid[11],
                        node->uuid[12], node->uuid[13], node->uuid[14], node->uuid[15]);
                scan_msg.protocol = GW_NODE_BLE_MESH;
                memcpy(scan_msg.protocol_info.ble_mesh_info.dev_addr, node->dev_addr, 6);
                memcpy(scan_msg.protocol_info.ble_mesh_info.uuid, node->uuid, 16);
                scan_msg.protocol_info.ble_mesh_info.addr_type = node->addr_type;
                scan_msg.protocol_info.ble_mesh_info.bearer    = node->bearer;
                scan_msg.protocol_info.ble_mesh_info.oob_info  = node->oob_info;
                if (g_gateway_mgmt_callback.mgmt_update_event_cb) {
                    g_gateway_mgmt_callback.mgmt_update_event_cb(&scan_msg, sizeof(scan_msg), GW_MSG_SCAN_DEV);
                }
            }
        } break;
        case BT_MESH_EVENT_FOUND_DEV_TIMEOUT: {
            if (g_gateway_mgmt_callback.mgmt_update_event_cb) {
                g_gateway_mgmt_callback.mgmt_update_event_cb(NULL, 0, GW_MSG_SCAN_DEV_TIMEOUT);
            }
        } break;
        case BT_MESH_EVENT_PROV_FAILED_WITH_INFO: {
            prov_failed_info_t fail_info     = *(prov_failed_info_t *)p_arg;
            gw_evt_bind_fail_t gw_fail_event = { 0x00 };
            gw_fail_event.protocol           = GW_NODE_BLE_MESH;
            memcpy(gw_fail_event.protocol_info.ble_mesh_info.dev_addr, fail_info.addr, 6);
            memcpy(gw_fail_event.protocol_info.ble_mesh_info.uuid, fail_info.uuid, 16);
            gw_fail_event.protocol_info.ble_mesh_info.addr_type = fail_info.addr_type;
            gw_fail_event.protocol_info.ble_mesh_info.bearer    = fail_info.bearer;
            gw_fail_event.protocol_info.ble_mesh_info.oob_info  = 0; /* Todo no oob info */
            gw_fail_event.failed_reason                         = fail_info.reason;
            LOGE(TAG, "Mesh dev %s prov failed, failed reason:%02x",
                 bt_hex_real(gw_fail_event.protocol_info.ble_mesh_info.dev_addr, 6), fail_info.reason);
            if (g_gateway_mgmt_callback.mgmt_update_event_cb) {
                g_gateway_mgmt_callback.mgmt_update_event_cb(&gw_fail_event, sizeof(gw_evt_bind_fail_t),
                                                             GW_MSG_ADD_SUB_DEV_FAIL);
            }
        } break;
        case BT_MESH_EVENT_OOB_INPUT_STATIC_OOB: {
            LOGD(TAG, "static oob input");
        } break;
        case BT_MESH_EVENT_OOB_INPUT_NUM_WITH_INFO: {
            if (p_arg) {
                oob_input_info_t info                 = *(oob_input_info_t *)p_arg;
                gw_auth_input_t  auth_info            = { 0x0 };
                auth_info.procotol                    = GW_NODE_BLE_MESH;
                auth_info.auth_ctx.mesh_ctx.action    = GW_AUTH_INPUT_NUM;
                auth_info.auth_ctx.mesh_ctx.size      = info.size;
                auth_info.auth_ctx.mesh_ctx.addr_type = info.addr_type;
                memcpy(auth_info.auth_ctx.mesh_ctx.dev_addr, info.addr, 6);
                memcpy(auth_info.auth_ctx.mesh_ctx.uuid, info.uuid, 16);
                LOGD(TAG, "oob input num");
                if (g_gateway_mgmt_callback.mgmt_update_event_cb) {
                    g_gateway_mgmt_callback.mgmt_update_event_cb(&auth_info, sizeof(gw_auth_input_t),
                                                                 GW_MSG_AUTH_INPUT);
                }
            }
        } break;
        case BT_MESH_EVENT_OOB_INPUT_STRING_WITH_INFO: {
            if (p_arg) {
                oob_input_info_t info                 = *(oob_input_info_t *)p_arg;
                gw_auth_input_t  auth_info            = { 0x0 };
                auth_info.procotol                    = GW_NODE_BLE_MESH;
                auth_info.auth_ctx.mesh_ctx.action    = GW_AUTH_INPUT_STR;
                auth_info.auth_ctx.mesh_ctx.size      = info.size;
                auth_info.auth_ctx.mesh_ctx.addr_type = info.addr_type;
                memcpy(auth_info.auth_ctx.mesh_ctx.dev_addr, info.addr, 6);
                memcpy(auth_info.auth_ctx.mesh_ctx.uuid, info.uuid, 16);
                LOGD(TAG, "oob input str");
                if (g_gateway_mgmt_callback.mgmt_update_event_cb) {
                    g_gateway_mgmt_callback.mgmt_update_event_cb(&auth_info, sizeof(gw_auth_input_t),
                                                                 GW_MSG_AUTH_INPUT);
                }
            }
        } break;
        case BT_MESH_EVENT_PROV_COMP: {
            if (p_arg) {
                mesh_node_t *node = (mesh_node_t *)p_arg;
                LOGD(TAG, "node add:%04x,%02x,%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\r\n",
                     node->prim_unicast, node->element_num, node->uuid[0], node->uuid[1], node->uuid[2], node->uuid[3],
                     node->uuid[4], node->uuid[5], node->uuid[6], node->uuid[7], node->uuid[8], node->uuid[9],
                     node->uuid[10], node->uuid[11], node->uuid[12], node->uuid[13], node->uuid[14], node->uuid[15]);
                (void)node;
            }
        } break;
        default:
            break;
    }
}

static void gateway_mesh_vendor_status_message_process(model_message message)
{
    if (message.ven_data.data_len < 2) {
        return;
    }
    uint8_t *data      = (uint8_t *)message.ven_data.user_data;
    uint16_t status_op = data[0] | data[1] << 8;

    switch (status_op) {
        case ATTR_TYPE_REPORT_VERSION: {
            uint32_t version = (data[2] & 0x00) | data[3] << 16 | data[4] << 8 | data[5];
            LOGD(TAG, "NODE:%04x,Version:%06x,Ota protocol:%02x", message.source_addr, version, data[6]);
            struct bt_mesh_node_t *node = NULL;

            node = bt_mesh_provisioner_get_node_info(message.source_addr);
            if (!node) {
                return;
            }

            if (node->version != version) {
                provisioner_node_version_set(bt_mesh_provisioner_get_node_id(node), version);
            }
            gw_subdev_version_status status = {
                .version          = version,
                .support_protocol = data[6],
            };
            gw_subdev_status_update_dev_info dev_info         = { 0x00 };
            dev_info.protocol                                 = GW_NODE_BLE_MESH;
            dev_info.protocol_info.ble_mesh_info.unicast_addr = message.source_addr;
            if (g_gateway_mgmt_callback.mgmt_update_status_cb) {
                g_gateway_mgmt_callback.mgmt_update_status_cb(
                    &dev_info, (void *)&status, sizeof(gw_subdev_version_status), GW_SUBDEV_VERSION_STATUS);
            }
        } break;
        case ATTR_TYPE_OVERWRITE_SUBLIST: {
            uint8_t status = data[2];
            LOGD(TAG, "NODE:%04x,Sublist overwrite status %02x\r\n", message.source_addr, status);
            (void)status;
        }
        case ATTR_TYPE_ACTIVE_CHECK: {
            uint8_t         node_lpm_type = data[2];
            hb_check_status status;
            memcpy(&status, &data[3], 1);
            uint8_t period_log = data[4];
            LOGD(TAG, "MESHACTIVESTATUS:%04x,%01x,%02x,%x,%x,%x,%x,%02x\r\n", message.source_addr, node_lpm_type,
                 period_log, status.hb_enable, status.hb_sub_pub_check, status.hb_ttl_check, status.hb_pub_count_check,
                 status.hb_period_check);
            struct bt_mesh_node_t *node = NULL;
            node                        = bt_mesh_provisioner_get_node_info(message.source_addr);
            if (!node) {
                return;
            }
            gw_active_check_status_t active_status = { 0x00 };
            active_status.node_period              = hb_pwr2(period_log, 1);
            if (node_lpm_type != BT_MESH_NODE_NO_LPM) {
                atomic_set_bit(&active_status.status, NODE_STATUS_IN_LPM);
            }
            if (status.hb_enable == NODE_HB_NOT_ENABLE) {
                LOGW(TAG, "Node:%04x hb not enable", message.source_addr);
                atomic_set_bit(&active_status.status, NODE_STATUS_HB_NOT_ENABLE);
            }
            if (status.hb_sub_pub_check == NODE_HB_SUB_PUB_MISMATCH) {
                LOGE(TAG, "You should sub the correct hb pub addr of Node:%04x at first", message.source_addr);
                atomic_set_bit(&active_status.status, NODE_STATUS_HB_PARAM_ERR);
            }
            if (status.hb_ttl_check == NODE_HB_TTL_INVALID) {
                LOGE(TAG, "Node:%04x pub at invalid ttl", message.source_addr);
                atomic_set_bit(&active_status.status, NODE_STATUS_HB_PARAM_ERR);
            }
            if (status.hb_pub_count_check == NODE_HB_COUNT_INVALID) {
                LOGE(TAG, "Node:%04x pub count is null", message.source_addr);
                atomic_set_bit(&active_status.status, NODE_STATUS_HB_PARAM_ERR);
            }
            if (status.hb_period_check == NODE_HB_PERIOD_MISMATCH) {
                atomic_set_bit(&active_status.status, NODE_STATUS_HB_PERIOD_MISS);
            }

            gw_subdev_status_update_dev_info dev_info         = { 0x00 };
            dev_info.protocol                                 = GW_NODE_BLE_MESH;
            dev_info.protocol_info.ble_mesh_info.unicast_addr = message.source_addr;

            if (node && g_gateway_mgmt_callback.mgmt_update_status_cb) {
                g_gateway_mgmt_callback.mgmt_update_status_cb(
                    &dev_info, &active_status, sizeof(gw_active_check_status_t), GW_SUBDEV_ACTIVE_CHECK_STATUS);
            }
        }
        default:
            break;
    }
}

static int gateway_mesh_node_hb_ttl_reconfig(struct bt_mesh_node_t *node, hb_status hb_set)
{
    uint8_t new_ttl    = hb_set.init_ttl - hb_set.recv_ttl + 2;
    uint8_t new_period = !node->hb_period_log ? CONFIG_GW_MESH_AUTOCONFIG_HB_SUB_PERIOD_LOG : node->hb_period_log;

    LOGD(TAG, "Node %s set new hb ttl:%d,period log:%d", bt_hex_real(node->addr_val, 6), new_ttl, new_period);
    struct bt_mesh_cfg_hb_pub pub = {
        .dst     = hb_set.dst_addr,
        .count   = CONFIG_BT_MESH_AUTOCONFIG_HB_PUB_COUNT_LOG,
        .period  = new_period,
        .ttl     = new_ttl,
        .feat    = hb_set.feat,
        .net_idx = hb_set.net_idx,
    };
    return bt_mesh_cfg_hb_pub_set(hb_set.net_idx, hb_set.src_addr, &pub, NULL);
}

static void gateway_mesh_model_cb(mesh_model_event_en event, void *p_arg)
{
    int                         ret   = 0;
    struct net_buf_simple_state state = { 0 };

    if (!p_arg) {
        return;
    }

    switch (event) {
        case BT_MESH_MODEL_CFG_APPKEY_STATUS: {
            model_message message = *(model_message *)p_arg;
            uint8_t       status  = message.status_data->data[0];
            if (status == 0 && bt_mesh_primary_addr() != message.source_addr) {
                LOGD(TAG, "appkey add:%04x,%x", message.source_addr, status);
                ret = gateway_mesh_prov_node_auto_config(message.source_addr);
                if (ret) {
                    LOGE(TAG, "Send auto config failed %d", ret);
                }
            }
        } break;

        case BT_MESH_MODEL_CFG_APPKEY_BIND_STATUS: {
            model_message message = *(model_message *)p_arg;
            uint8_t       status  = message.status_data->data[0];
            if (status) {
                LOGD(TAG, "appkey bind:%04x,%x", message.source_addr, status);
            }
        } break;

        case BT_MESH_MODEL_CFG_COMP_DATA_STATUS: {
            LOGD(TAG, "Get node comp data");
        } break;

        case BT_MESH_MODEL_CFG_SUB_STATUS: {
            model_message message = *(model_message *)p_arg;
            uint8_t       status  = message.status_data->data[0];
            if (status) {
                LOGD(TAG, "set sub:%04x,%x", message.source_addr, status);
            }
        } break;

        case BT_MESH_MODEL_CFG_SUB_LIST: {

            model_message message = *(model_message *)p_arg;
            if (message.status_data) {
                net_buf_simple_save(message.status_data, &state);
                uint8_t  status_data = (uint8_t)net_buf_simple_pull_u8(message.status_data);
                uint16_t addr        = (uint16_t)net_buf_simple_pull_le16(message.status_data);
                uint16_t mod_id      = (uint16_t)net_buf_simple_pull_le16(message.status_data);

                if (!status_data && message.status_data->len >= 2) {
                    uint8_t first_addr = 0;

                    while (message.status_data->len >= 2) {
                        if (!first_addr) {
                            LOGD(TAG, "sublist:%04x,%04x,%04x,%04x", addr, CID_NVAL, mod_id,
                                 (uint16_t)net_buf_simple_pull_le16(message.status_data));
                            first_addr = 1;
                        } else {
                            LOGD("", ",%04x", (uint16_t)net_buf_simple_pull_le16(message.status_data));
                        }
                    }

                    if (first_addr) {
                        LOGD("", "\r\n");
                    }

                } else if (status_data) {
                    LOGD(TAG, "sublist:%04x,%x", addr, status_data);
                } else if (!status_data && message.status_data->len < 2) {
                    LOGD(TAG, "sublist:%04x,%04X,%04x,NULL", addr, CID_NVAL, mod_id);
                }
                net_buf_simple_restore(message.status_data, &state);
                (void)addr;
                (void)mod_id;
            }

        } break;

        case BT_MESH_MODEL_CFG_SUB_LIST_VND: {

            model_message message = *(model_message *)p_arg;
            if (message.status_data) {
                net_buf_simple_save(message.status_data, &state);
                uint8_t  status_data = (uint8_t)net_buf_simple_pull_u8(message.status_data);
                uint16_t addr        = (uint16_t)net_buf_simple_pull_le16(message.status_data);
                uint16_t company_id  = (uint16_t)net_buf_simple_pull_le16(message.status_data);
                uint16_t mod_id      = (uint16_t)net_buf_simple_pull_le16(message.status_data);

                if (!status_data && message.status_data->len >= 2) {
                    uint8_t first_addr = 0;

                    while (message.status_data->len >= 2) {
                        if (!first_addr) {
                            LOGD(TAG, "sublist vnd:%04x,%04x,%04x,%04x", addr, company_id, mod_id,
                                 (uint16_t)net_buf_simple_pull_le16(message.status_data));
                            first_addr = 1;
                        } else {
                            LOGD("", ",%04x", (uint16_t)net_buf_simple_pull_le16(message.status_data));
                        }
                    }

                    if (first_addr) {
                        LOGD("", "\r\n");
                    }

                } else if (status_data) {
                    LOGD(TAG, "sublist vnd:%d", status_data);
                } else if (!status_data && message.status_data->len < 2) {
                    LOGD(TAG, "sublist vnd:%04x,%04x,%04x,NULL", addr, company_id, mod_id);
                }
                net_buf_simple_restore(message.status_data, &state);
                (void)addr;
                (void)company_id;
                (void)mod_id;
            }

        } break;

        case BT_MESH_MODEL_CFG_PUB_STATUS: {
            model_message message = *(model_message *)p_arg;
            if (message.status_data) {
                net_buf_simple_save(message.status_data, &state);
                uint8_t  status    = (uint8_t)net_buf_simple_pull_u8(message.status_data);
                uint16_t elem_addr = (uint16_t)net_buf_simple_pull_le16(message.status_data);
                if (status) {
                    LOGD(TAG, "setpub:%04x,%x", message.source_addr, status);
                    return;
                }

                if (message.status_data->len < 1) {
                    LOGD(TAG, "setpub:%04x,NULL", elem_addr);
                    return;
                }

                uint16_t pub_addr = (uint16_t)net_buf_simple_pull_le16(message.status_data);
                net_buf_simple_pull_le16(message.status_data);
                uint8_t  ttl        = (uint8_t)net_buf_simple_pull_u8(message.status_data);
                uint8_t  period     = (uint8_t)net_buf_simple_pull_u8(message.status_data);
                uint8_t  retransmit = (uint8_t)net_buf_simple_pull_u8(message.status_data);
                uint16_t CID;
                uint16_t mod_id;

                if (message.status_data->len >= 4) {
                    CID    = (uint16_t)net_buf_simple_pull_le16(message.status_data);
                    mod_id = (uint16_t)net_buf_simple_pull_le16(message.status_data);
                } else {
                    CID    = CID_NVAL;
                    mod_id = (uint16_t)net_buf_simple_pull_le16(message.status_data);
                }

                if (BT_MESH_ADDR_UNASSIGNED == pub_addr) {
                    LOGD(TAG, "setpub:%04x,%04x,%04x,NULL", elem_addr, CID, mod_id);
                } else {
                    LOGD(TAG, "setpub:%04x,%04x,%04x,%04x,%02x,%02x,%02x,%02x", elem_addr, CID, mod_id, pub_addr, ttl,
                         period, BT_MESH_PUB_TRANSMIT_COUNT(retransmit), BT_MESH_PUB_TRANSMIT_INT(retransmit));
                }
                net_buf_simple_restore(message.status_data, &state);
                (void)elem_addr;
                (void)ttl;
                (void)period;
                (void)retransmit;
                (void)CID;
                (void)mod_id;
            }
        } break;

        case BT_MESH_MODEL_VENDOR_MESH_AUTOCONFIG_STATUS: {
            model_message *        message  = (model_message *)p_arg;
            uint8_t *              ven_data = (uint8_t *)message->ven_data.user_data;
            uint8_t                status   = ven_data[1];
            struct bt_mesh_node_t *node     = NULL;

            if (0 == status) {
                LOGD(TAG, "auto config:%04x,%d", message->source_addr, 0);

                node = bt_mesh_provisioner_get_node_info(message->source_addr);
                if (!node) {
                    return;
                }

                gateway_add_subdev_msg_t msg = { 0 };
                if (node->dev_uuid[13] == BT_MESH_NODE_NO_LPM) {
                    msg.dev_type.lpm_type = GW_DEV_NO_LPM;
                } else if (node->dev_uuid[13] == BT_MESH_NODE_LPM_NO_RX) {
                    msg.dev_type.lpm_type = GW_DEV_LPM_NO_RX;
                } else if (node->dev_uuid[13] == BT_MESH_NODE_LPM_RX_TX) {
                    msg.dev_type.lpm_type = GW_DEV_LPM_RX_TX;
                } else if (node->dev_uuid[13] == BT_MESH_NODE_LPM_TX_RX) {
                    msg.dev_type.lpm_type = GW_DEV_LPM_TX_RX;
                }
                msg.dev_type.protocol  = GW_NODE_BLE_MESH;
                msg.dev_type.node_type = GW_DEV_NODE;
                memcpy(msg.protocol_info.ble_mesh_info.dev_addr, node->addr_val, 6);
                memcpy(msg.protocol_info.ble_mesh_info.uuid, node->dev_uuid, 16);
                msg.protocol_info.ble_mesh_info.addr_type    = node->addr_type;
                msg.protocol_info.ble_mesh_info.oob_info     = node->oob_info;
                msg.protocol_info.ble_mesh_info.unicast_addr = node->unicast_addr;
                msg.protocol_info.ble_mesh_info.elem_num     = node->element_num;
                msg.retry_time                               = 0;
                if (g_gateway_mgmt_callback.mgmt_update_event_cb) {
                    g_gateway_mgmt_callback.mgmt_update_event_cb(&msg, sizeof(gateway_add_subdev_msg_t),
                                                                 GW_MSG_ADD_SUB_DEV);
                }
                ble_mesh_node_get_node_app_version(0, 0, message->source_addr);
            } else {
                LOGD(TAG, "auto config:%04x,%x", message->source_addr, status);
                node = bt_mesh_provisioner_get_node_info(message->source_addr);
                if (!node) {
                    return;
                }
            }
        } break;

        // model status
        case BT_MESH_MODEL_CFG_RELAY_STATUS: {
            model_message relay_message = *(model_message *)p_arg;
            LOGD(TAG, "relay:%04x,%02x", relay_message.source_addr, relay_message.status_data->data[0]);
            (void)relay_message;
        } break;

        case BT_MESH_MODEL_CFG_PROXY_STATUS: {

            model_message proxy_message = *(model_message *)p_arg;
            LOGD(TAG, "proxy:%04x,%02x", proxy_message.source_addr, proxy_message.status_data->data[0]);
            (void)proxy_message;
        } break;

        case BT_MESH_MODEL_CFG_FRIEND_STATUS: {
            model_message friend_message = *(model_message *)p_arg;
            LOGD(TAG, "friend:%04x,%02x", friend_message.source_addr, friend_message.status_data->data[0]);
            (void)friend_message;
        } break;

        case BT_MESH_MODEL_CFG_RST_STATUS: {

            model_message          rst_message = *(model_message *)p_arg;
            struct bt_mesh_node_t *node        = NULL;
            // uint8_t dev_addr_str[20] = {0};

            LOGD(TAG, "rst:%04x", rst_message.source_addr);
            node = bt_mesh_provisioner_get_node_info(rst_message.source_addr);
            if (!node) {
                return;
            }
            gw_subdev_rst_status rst_status                     = { 0x00 };
            rst_status.protocol                                 = GW_NODE_BLE_MESH;
            rst_status.protocol_info.ble_mesh_info.unicast_addr = rst_message.source_addr;
            if (g_gateway_mgmt_callback.mgmt_update_event_cb) {
                g_gateway_mgmt_callback.mgmt_update_event_cb(&rst_status, sizeof(gw_subdev_rst_status),
                                                             GW_MSG_DEL_SUB_DEV);
            }
        } break;
        case BT_MESH_MODEL_CFG_HEARTBEAT_SUB_STATUS: {
            if (p_arg) {
                model_message hb_sub_status_message = *(model_message *)p_arg;
                net_buf_simple_save(hb_sub_status_message.status_data, &state);
                uint8_t  status     = net_buf_simple_pull_u8(hb_sub_status_message.status_data);
                uint16_t sub_src    = net_buf_simple_pull_le16(hb_sub_status_message.status_data);
                uint16_t sub_dst    = net_buf_simple_pull_le16(hb_sub_status_message.status_data);
                uint8_t  period_log = net_buf_simple_pull_u8(hb_sub_status_message.status_data);
                uint8_t  count_log  = net_buf_simple_pull_u8(hb_sub_status_message.status_data);
                uint8_t  min_hop    = net_buf_simple_pull_u8(hb_sub_status_message.status_data);
                uint8_t  max_hop    = net_buf_simple_pull_u8(hb_sub_status_message.status_data);
                if (status) {
                    LOGE(TAG, "meshhbsub:%04x,%04x,%04x,%02x,%02x,%02x,%02x,%x", hb_sub_status_message.source_addr,
                         sub_src, sub_dst, period_log, count_log, min_hop, max_hop, status);
                } else {
                    LOGD(TAG, "meshhbsub:%04x,%04x,%02x,%02x,%02x,%02x,%02x,%x", hb_sub_status_message.source_addr,
                         sub_src, sub_dst, period_log, count_log, min_hop, max_hop, 0);
                }
                net_buf_simple_restore(hb_sub_status_message.status_data, &state);
                gw_subdev_hb_mgmt_set_status_t gw_hb_mgmt_status = { 0x0 };
                if (sub_dst == BT_MESH_ADDR_UNASSIGNED) {
                    LOGD(TAG, "Hb sub disable status:%02x", status);
                    gw_hb_mgmt_status.status = (status == 0 ? GW_HB_MGMT_DISABLE_SUCCESS : GW_HB_MGMT_DISABLE_FAILED);
                } else {
                    LOGD(TAG, "Hb sub enable status:%02x", status);
                    gw_hb_mgmt_status.status = (status == 0 ? GW_HB_MGMT_ENABLE_SUCCESS : GW_HB_MGMT_ENABLE_FAILED);
                }
                if (g_gateway_mgmt_callback.mgmt_update_event_cb) {
                    g_gateway_mgmt_callback.mgmt_update_event_cb(
                        &gw_hb_mgmt_status, sizeof(gw_subdev_hb_mgmt_set_status_t), GW_MSG_DEV_HB_MGMT_SET);
                }
            }
        } break;
        case BT_MESH_MODEL_CFG_HEARTBEAT_PUB_STATUS: {
            if (p_arg) {
                model_message          hb_pub_status_message = *(model_message *)p_arg;
                struct bt_mesh_node_t *node                  = NULL;
                uint16_t               period;
                net_buf_simple_save(hb_pub_status_message.status_data, &state);
                uint8_t  status      = net_buf_simple_pull_u8(hb_pub_status_message.status_data);
                uint16_t pub_dst     = net_buf_simple_pull_le16(hb_pub_status_message.status_data);
                uint8_t  pub_count   = net_buf_simple_pull_u8(hb_pub_status_message.status_data);
                uint8_t  pub_period  = net_buf_simple_pull_u8(hb_pub_status_message.status_data);
                uint8_t  pub_ttl     = net_buf_simple_pull_u8(hb_pub_status_message.status_data);
                uint8_t  pub_feat    = net_buf_simple_pull_u8(hb_pub_status_message.status_data);
                uint8_t  pub_net_idx = net_buf_simple_pull_u8(hb_pub_status_message.status_data);
                node                 = bt_mesh_provisioner_get_node_info(hb_pub_status_message.source_addr);
                if (!node) {
                    return;
                }
                gw_subdev_hb_set_status_t hb_set_status;
                if (pub_dst == BT_MESH_ADDR_UNASSIGNED) {
                    LOGD(TAG, "Hb pub disable status:%02x", status);
                    hb_set_status.status = (status == 0 ? GW_HB_DISABLE_SUCCESS : GW_HB_DISABLE_FAILED);
                } else {
                    LOGD(TAG, "Hb pub enable status:%02x", status);
                    hb_set_status.status = (status == 0 ? GW_HB_ENABLE_SUCCESS : GW_HB_ENABLE_FAILED);
                }

                hb_set_status.period_s                            = hb_pwr2(pub_period, 1);
                gw_subdev_status_update_dev_info dev_info         = { 0x00 };
                dev_info.protocol                                 = GW_NODE_BLE_MESH;
                dev_info.protocol_info.ble_mesh_info.unicast_addr = hb_pub_status_message.source_addr;
                if (g_gateway_mgmt_callback.mgmt_update_status_cb) {
                    g_gateway_mgmt_callback.mgmt_update_status_cb(
                        &dev_info, &hb_set_status, sizeof(gw_subdev_hb_set_status_t), GW_SUBDEV_HB_SET_STATUS);
                }
                (void)pub_net_idx;
                (void)pub_feat;
                (void)pub_ttl;
                (void)pub_count;
                (void)period;
                net_buf_simple_restore(hb_pub_status_message.status_data, &state);
            }
        } break;

        case BT_MESH_MODEL_ONOFF_STATUS: {
            model_message          onoff_message = *(model_message *)p_arg;
            struct bt_mesh_node_t *node          = NULL;
            LOGD(TAG, "onoff:%04x,%02x", onoff_message.source_addr, onoff_message.status_data->data[0]);
            node = bt_mesh_provisioner_get_node_info(onoff_message.source_addr);
            if (!node) {
                return;
            }

            gw_subdev_status_update_dev_info dev_info         = { 0x00 };
            dev_info.protocol                                 = GW_NODE_BLE_MESH;
            dev_info.protocol_info.ble_mesh_info.unicast_addr = onoff_message.source_addr;

            if (g_gateway_mgmt_callback.mgmt_update_status_cb) {
                g_gateway_mgmt_callback.mgmt_update_status_cb(&dev_info, (void *)&(onoff_message.status_data->data[0]),
                                                              sizeof(uint8_t), GW_SUBDEV_ONOFF_STATUS);
            }
        } break;

        case BT_MESH_MODEL_LEVEL_STATUS: {
            model_message level_message = *(model_message *)p_arg;
            if (level_message.status_data) {
                net_buf_simple_save(level_message.status_data, &state);
                int16_t level = (int16_t)net_buf_simple_pull_le16(level_message.status_data);
                net_buf_simple_restore(level_message.status_data, &state);
                LOGD(TAG, "level:%04x,%04x", level_message.source_addr, level);
                gw_subdev_status_update_dev_info dev_info         = { 0x00 };
                dev_info.protocol                                 = GW_NODE_BLE_MESH;
                dev_info.protocol_info.ble_mesh_info.unicast_addr = level_message.source_addr;

                if (g_gateway_mgmt_callback.mgmt_update_status_cb) {
                    g_gateway_mgmt_callback.mgmt_update_status_cb(&dev_info, (void *)&level, sizeof(level),
                                                                  GW_SUBDEV_LEVEL_STATUS);
                }
            }

        } break;
        case BT_MESH_MODEL_CFG_HEARTBEAT_STATUS: {
            if (p_arg) {
                hb_status              hb_message = *(hb_status *)p_arg;
                int                    ret        = 0;
                struct bt_mesh_node_t *node       = bt_mesh_provisioner_get_node_info(hb_message.src_addr);
                if (node) {
                    if (hb_message.recv_ttl >= 3) {
                        ret = gateway_mesh_node_hb_ttl_reconfig(node, hb_message);
                        if (ret) {
                            LOGE(TAG, "hb ttl reconfig failed %d", ret);
                        }
                    }
                    gw_subdev_status_update_dev_info dev_info         = { 0x00 };
                    dev_info.protocol                                 = GW_NODE_BLE_MESH;
                    dev_info.protocol_info.ble_mesh_info.unicast_addr = hb_message.src_addr;

                    if (g_gateway_mgmt_callback.mgmt_update_status_cb) {
                        g_gateway_mgmt_callback.mgmt_update_status_cb(&dev_info, NULL, 0, GW_SUBDEV_HB_STATUS);
                    }
                }
            }
        } break;
        case BT_MESH_MODEL_LIGHTNESS_STATUS: {

            model_message          lightness_message = *(model_message *)p_arg;
            struct bt_mesh_node_t *node              = NULL;

            node = bt_mesh_provisioner_get_node_info(lightness_message.source_addr);
            if (!node) {
                return;
            }
            if (lightness_message.status_data) {
                net_buf_simple_save(lightness_message.status_data, &state);
                int16_t lightness = (int16_t)net_buf_simple_pull_le16(lightness_message.status_data);
                net_buf_simple_restore(lightness_message.status_data, &state);
                LOGD(TAG, "lightness:%04x,%04x", lightness_message.source_addr, lightness);
                gw_subdev_status_update_dev_info dev_info         = { 0x00 };
                dev_info.protocol                                 = GW_NODE_BLE_MESH;
                dev_info.protocol_info.ble_mesh_info.unicast_addr = lightness_message.source_addr;
                if (g_gateway_mgmt_callback.mgmt_update_status_cb) {
                    g_gateway_mgmt_callback.mgmt_update_status_cb(&dev_info, (void *)&lightness, sizeof(lightness),
                                                                  GW_SUBDEV_BRIGHTNESS_STATUS);
                }
            }

        } break;

        case BT_MESH_MODEL_LIGHTNESS_LINEAR_STATUS: {

            model_message lightness_lin_message = *(model_message *)p_arg;
            if (lightness_lin_message.status_data) {
                net_buf_simple_save(lightness_lin_message.status_data, &state);
                int16_t lightness_lin = (int16_t)net_buf_simple_pull_le16(lightness_lin_message.status_data);
                net_buf_simple_restore(lightness_lin_message.status_data, &state);
                LOGD(TAG, "lightness lin:%04x,%04x", lightness_lin_message.source_addr, lightness_lin);
                (void)lightness_lin;
            }
            (void)lightness_lin_message;
        } break;

        case BT_MESH_MODEL_LIGHTNESS_RANGE_STATUS: {

            model_message lightness_range_message = *(model_message *)p_arg;
            if (lightness_range_message.status_data) {
                net_buf_simple_save(lightness_range_message.status_data, &state);
                uint8_t status_code = (uint8_t)net_buf_simple_pull_u8(lightness_range_message.status_data);
                int16_t range_min   = (int16_t)net_buf_simple_pull_le16(lightness_range_message.status_data);
                int16_t range_max   = (int16_t)net_buf_simple_pull_le16(lightness_range_message.status_data);
                net_buf_simple_restore(lightness_range_message.status_data, &state);
                LOGD(TAG, "lightness range:%04x,%02x,%04x,%04x", lightness_range_message.source_addr, status_code,
                     range_min, range_max);
                (void)status_code;
                (void)range_min;
                (void)range_max;
            }

        } break;

        case BT_MESH_MODEL_LIGHTNESS_DEF_STATUS: {

            model_message lightness_def_message = *(model_message *)p_arg;
            if (lightness_def_message.status_data) {
                net_buf_simple_save(lightness_def_message.status_data, &state);
                int16_t lightness_def = (int16_t)net_buf_simple_pull_le16(lightness_def_message.status_data);
                net_buf_simple_restore(lightness_def_message.status_data, &state);
                LOGD(TAG, "lightness def:%04x,%04x", lightness_def_message.source_addr, lightness_def);
                (void)lightness_def;
            }
        } break;

        case BT_MESH_MODEL_LIGHTNESS_LAST_STATUS: {

            model_message lightness_last_message = *(model_message *)p_arg;
            int16_t       lightness_last = (int16_t)net_buf_simple_pull_le16(lightness_last_message.status_data);
            LOGD(TAG, "lightness last:%04x,%04x", lightness_last_message.source_addr, lightness_last);
            (void)lightness_last_message;
            (void)lightness_last;

        } break;

        case BT_MESH_MODEL_LIGHT_CTL_STATUS: {

            model_message light_ctl_message = *(model_message *)p_arg;
            if (light_ctl_message.status_data) {
                net_buf_simple_save(light_ctl_message.status_data, &state);
                int16_t lightness = (int16_t)net_buf_simple_pull_le16(light_ctl_message.status_data);
                int16_t ctl_temp  = (int16_t)net_buf_simple_pull_le16(light_ctl_message.status_data);
                net_buf_simple_restore(light_ctl_message.status_data, &state);
                LOGD(TAG, "mesh ctl:%04x,%04x,%04x", light_ctl_message.source_addr, lightness, ctl_temp);
                (void)lightness;
                (void)ctl_temp;
            }

        } break;

        case BT_MESH_MODEL_LIGHT_CTL_TEMP_STATUS: {

            model_message          light_ctl_temp_message = *(model_message *)p_arg;
            struct bt_mesh_node_t *node                   = NULL;

            node = bt_mesh_provisioner_get_node_info(light_ctl_temp_message.source_addr);
            if (!node) {
                return;
            }
            if (light_ctl_temp_message.status_data) {
                net_buf_simple_save(light_ctl_temp_message.status_data, &state);
                int16_t ctl_temp = (int16_t)net_buf_simple_pull_le16(light_ctl_temp_message.status_data);
                int16_t uv       = (int16_t)net_buf_simple_pull_le16(light_ctl_temp_message.status_data);
                net_buf_simple_restore(light_ctl_temp_message.status_data, &state);
                LOGD(TAG, "mesh ctl temp:%04x,%04x,%04x", light_ctl_temp_message.source_addr, ctl_temp, uv);
                gw_subdev_status_update_dev_info dev_info         = { 0x00 };
                dev_info.protocol                                 = GW_NODE_BLE_MESH;
                dev_info.protocol_info.ble_mesh_info.unicast_addr = light_ctl_temp_message.source_addr;
                if (g_gateway_mgmt_callback.mgmt_update_status_cb) {
                    g_gateway_mgmt_callback.mgmt_update_status_cb(&dev_info, (void *)&ctl_temp, sizeof(ctl_temp),
                                                                  GW_SUBDEV_CCT_STATUS);
                }
                (void)ctl_temp;
                (void)uv;
            }

        } break;

        case BT_MESH_MODEL_LIGHT_CTL_TEMP_RANGE_STATUS: {

            model_message light_ctl_temp_ran_message = *(model_message *)p_arg;
            if (light_ctl_temp_ran_message.status_data) {
                net_buf_simple_save(light_ctl_temp_ran_message.status_data, &state);
                uint8_t status_code = (uint8_t)net_buf_simple_pull_u8(light_ctl_temp_ran_message.status_data);
                int16_t temp_min    = (int16_t)net_buf_simple_pull_le16(light_ctl_temp_ran_message.status_data);
                int16_t temp_max    = (int16_t)net_buf_simple_pull_le16(light_ctl_temp_ran_message.status_data);
                net_buf_simple_restore(light_ctl_temp_ran_message.status_data, &state);
                LOGD(TAG, "mesh ctl temp range:%04x,%02x,%04x,%04x", light_ctl_temp_ran_message.source_addr,
                     status_code, temp_min, temp_max);
                (void)status_code;
                (void)temp_min;
                (void)temp_max;
            }

        } break;

        case BT_MESH_MODEL_LIGHT_CTL_DEF_STATUS: {

            model_message light_ctl_def_message = *(model_message *)p_arg;
            if (light_ctl_def_message.status_data) {
                net_buf_simple_save(light_ctl_def_message.status_data, &state);
                int16_t lightness = (int16_t)net_buf_simple_pull_le16(light_ctl_def_message.status_data);
                int16_t ctl_temp  = (int16_t)net_buf_simple_pull_le16(light_ctl_def_message.status_data);
                int16_t uv        = (int16_t)net_buf_simple_pull_le16(light_ctl_def_message.status_data);
                net_buf_simple_restore(light_ctl_def_message.status_data, &state);
                LOGD(TAG, "ctl def:%04x,%04x,%04x,%04x", light_ctl_def_message.source_addr, lightness, ctl_temp, uv);
                (void)lightness;
                (void)ctl_temp;
                (void)uv;
            }

        } break;

        case BT_MESH_MODEL_VENDOR_MESSAGES: {
            model_message          message      = *(model_message *)p_arg;
            uint16_t               unicast_addr = message.source_addr;
            uint8_t                len          = message.ven_data.data_len;
            char *                 data         = (char *)message.ven_data.user_data;
            struct bt_mesh_node_t *node         = NULL;
            node                                = bt_mesh_provisioner_get_node_info(message.source_addr);
            if (!node) {
                return;
            }

            LOGD(TAG, "mesh trs:%04x,%02x,%s", unicast_addr, len, bt_hex(data, len));
            gw_subdev_status_update_dev_info dev_info         = { 0x00 };
            dev_info.protocol                                 = GW_NODE_BLE_MESH;
            dev_info.protocol_info.ble_mesh_info.unicast_addr = message.source_addr;
            if (!memcmp(data, TRIGET_FLAG, strlen(TRIGET_FLAG))) {
                LOGD(TAG, "Get triples data,status %d", data[strlen(TRIGET_FLAG)]);
                uint8_t                        offset      = strlen(TRIGET_FLAG);
                gw_occ_subdev_triples_status_t status_data = { 0x0 };
                status_data.status                         = data[offset++];
                if (!status_data.status) {
                    if (data[offset] & TRIPLE_TYPE_DN) {
                        memcpy(status_data.status_data.device_name, data + offset + 2, data[offset + 1]);
                        offset += data[offset + 1] + 2;
                        status_data.flag |= TRIPLE_TYPE_DN;
                    }

                    if (data[offset] & TRIPLE_TYPE_PK) {
                        memcpy(status_data.status_data.product_key, data + offset + 2, data[offset + 1]);
                        offset += data[offset + 1] + 2;
                        status_data.flag |= TRIPLE_TYPE_PK;
                    }

                    if (data[offset] & TRIPLE_TYPE_PS) {
                        memcpy(status_data.status_data.product_secret, data + offset + 2, data[offset + 1]);
                        offset += data[offset + 1] + 2;
                        status_data.flag |= TRIPLE_TYPE_PS;
                    }
                }

                if (g_gateway_mgmt_callback.mgmt_update_status_cb) {
                    g_gateway_mgmt_callback.mgmt_update_status_cb(&dev_info, (void *)&status_data,
                                                                  sizeof(gw_occ_subdev_triples_status_t),
                                                                  GW_SUBDEV_TRIPLES_STATUS);
                }
            } else {
                if (g_gateway_mgmt_callback.mgmt_update_status_cb) {
                    g_gateway_mgmt_callback.mgmt_update_status_cb(&dev_info, (void *)&data, len, GW_SUBDEV_RAW_DATA);
                }
            }
            (void)unicast_addr;
        } break;

        case BT_MESH_MODEL_VENDOR_MESH_STATUS: {
            model_message message = *(model_message *)p_arg;
            if (message.status_data) {
                net_buf_simple_save(message.status_data, &state);
                gateway_mesh_vendor_status_message_process(message);
                net_buf_simple_restore(message.status_data, &state);
            }
        } break;

        case BT_MESH_MODEL_ONOFF_SET:
        case BT_MESH_MODEL_ONOFF_SET_UNACK: {
            model_message message    = *(model_message *)p_arg;
            S_ELEM_STATE *elem_state = (S_ELEM_STATE *)message.user_data;
            LOGD(TAG, "onoff set:%02x\r\n", elem_state->state.onoff[T_TAR]);

            gw_subdev_status_update_dev_info dev_info         = { 0x00 };
            dev_info.protocol                                 = GW_NODE_BLE_MESH;
            dev_info.protocol_info.ble_mesh_info.unicast_addr = message.source_addr;

            if (g_gateway_mgmt_callback.mgmt_update_status_cb) {
                g_gateway_mgmt_callback.mgmt_update_status_cb(&dev_info, (void *)&(elem_state->state.onoff[T_TAR]),
                                                              sizeof(uint8_t), GW_SUBDEV_ONOFF_STATUS);
            }
        } break;

        default:
            break;
    }
}

/* api for user */

node_config_t g_node_param = {
    .dev_name      = GW_MESH_DEV_NAME,
    .user_model_cb = gateway_mesh_model_cb,
    .user_prov_cb  = NULL,
};

int gateway_mesh_init()
{
    static uint8_t init_flag = 0;
    int            err       = 0;

    if (init_flag == 1) {
        LOGD(TAG, "mesh already inited");
        return 0;
    }

    err = ble_mesh_model_init(&mesh_comp);
    if (err) {
        LOGE(TAG, "mesh comp init failed %d\r\n", err);
        return err;
    }

    err = ble_mesh_node_init(&g_node_param);
    if (err < 0) {
        LOGE(TAG, "mesh node init failed %d\r\n", err);
    } else if (err == 0) {
        LOGD(TAG, "mesh node init ,and it has not been proved ");
    } else if (err == 1) {
        LOGD(TAG, "mesh node init ,and it has been proved");
    }

    err = ble_mesh_node_provisioner_init(gateway_mesh_provisioner_cb);
    if (err) {
        LOGE(TAG, "mesh node provisioner init failed %d\r\n", err);
    } else {
        LOGD(TAG, "mesh node provisioner init success");
    }

    if (!bt_mesh_is_provisioned()) {
        uint16_t                 unicast_addr = 0;
        uint8_t                  elem_num     = 0;
        gateway_add_subdev_msg_t msg          = { 0 };

        err = ble_mesh_node_provisioner_config(&g_provisioner_param);
        if (err) {
            LOGE(TAG, "Provisioner config failed %d", err);
            return err;
        }
        err = ble_mesh_node_provisioner_autoprov();
        if (err) {
            LOGE(TAG, "Provisioner autoprov failed %d", err);
            return err;
        }
        err = ble_mesh_node_provisioner_enable();
        if (err) {
            LOGE(TAG, "Provisioner autoprov failed %d", err);
            return err;
        }
        LOGD(TAG, "Provisioner init success, start addr:0x%04x, end addr:0x%04x",
             g_provisioner_param.unicast_start_addr, g_provisioner_param.unicast_end_addr);

        bt_mesh_provisioner_get_own_unicast_addr(&unicast_addr, &elem_num);

        msg.dev_type.lpm_type                        = GW_DEV_NO_LPM;
        msg.dev_type.protocol                        = GW_NODE_BLE_MESH;
        msg.dev_type.node_type                       = GW_DEV_GATEWAY;
        msg.protocol_info.ble_mesh_info.unicast_addr = unicast_addr;
        msg.protocol_info.ble_mesh_info.elem_num     = elem_num;
        msg.retry_time                               = 0;
        if (g_gateway_mgmt_callback.mgmt_update_event_cb) {
            g_gateway_mgmt_callback.mgmt_update_event_cb(&msg, sizeof(gateway_add_subdev_msg_t), GW_MSG_ADD_SUB_DEV);
        }
    }

    if (err == 0) {
        init_flag = 1;
    }

    return err;
}

void gateway_mesh_mgmt_cb_register(gateway_mgmt_callback_t gw_cb)
{
    g_gateway_mgmt_callback.mgmt_update_event_cb  = gw_cb.mgmt_update_event_cb;
    g_gateway_mgmt_callback.mgmt_update_status_cb = gw_cb.mgmt_update_status_cb;
}

int gateway_mesh_suspend()
{
    bt_mesh_prov_disable(BT_MESH_PROV_GATT | BT_MESH_PROV_ADV);
    return bt_mesh_suspend(1);
}

int gateway_mesh_resume()
{
    bt_mesh_prov_enable(BT_MESH_PROV_GATT | BT_MESH_PROV_ADV);
    return bt_mesh_resume();
}

int gateway_mesh_hb_recv_enable(uint8_t enable)
{
    if (enable) {
        return bt_mesh_mod_hb_sub_set(0, CONFIG_GW_MESH_AUTOCONFIG_HB_SUB_SRC_ADDR,
                                      CONFIG_GW_MESH_AUTOCONFIG_HB_SUB_DST_ADDR,
                                      CONFIG_GW_MESH_AUTOCONFIG_HB_SUB_PERIOD_LOG);
    } else {
        return bt_mesh_mod_hb_sub_set(0, BT_MESH_ADDR_UNASSIGNED, BT_MESH_ADDR_UNASSIGNED, 0x00);
    }
}

int gateway_mesh_hb_pub_set(uint16_t unicast_addr, uint8_t enable, uint16_t period_s)
{
    struct bt_mesh_cfg_hb_pub pub;

    if (enable) {
        pub.dst     = CONFIG_GW_MESH_AUTOCONFIG_HB_SUB_DST_ADDR;
        pub.count   = CONFIG_BT_MESH_AUTOCONFIG_HB_PUB_COUNT_LOG;
        pub.period  = hb_get_log(period_s, 1);
        pub.ttl     = CONFIG_GW_MESH_AUTOCONFIG_HB_PUB_TTL;
        pub.feat    = CONFIG_GW_MESH_AUTOCONFIG_HB_PUB_FEAD;
        pub.net_idx = CONFIG_GW_MESH_AUTOCONFIG_HB_PUB_NET_IDX;
    } else {
        pub.dst     = BT_MESH_ADDR_UNASSIGNED;
        pub.count   = 0x00;
        pub.period  = 0x00;
        pub.ttl     = CONFIG_GW_MESH_AUTOCONFIG_HB_PUB_TTL;
        pub.feat    = CONFIG_GW_MESH_AUTOCONFIG_HB_PUB_FEAD;
        pub.net_idx = CONFIG_GW_MESH_AUTOCONFIG_HB_PUB_NET_IDX;
    }
    return bt_mesh_cfg_hb_pub_set(0, unicast_addr, &pub, NULL);
}

int gateway_mesh_node_version_get(uint16_t unicast_addr)
{
    return ble_mesh_node_get_node_app_version(0, 0, unicast_addr);
}

int gateway_mesh_node_triples_get(uint16_t unicast_addr, uint8_t triples_flag)
{
    char get[10] = { 0x0 };
    snprintf(get, sizeof(get), "triget:%01x", triples_flag);
    return gateway_mesh_send_rawdata(unicast_addr, (uint8_t *)get, strlen(get));
}

int gateway_mesh_hb_config_check(uint16_t unicast_addr, uint16_t period)

{
    uint8_t period_log;
    period_log = hb_get_log(period, 1);
    return ble_mesh_node_active_check(0, 0, unicast_addr, period_log);
}

int gateway_mesh_set_onoff(uint16_t unicast_addr, uint8_t onoff)
{
    struct bt_mesh_model *onoff_cli_model;
    onoff_cli_model = ble_mesh_model_find(0, BT_MESH_MODEL_ID_GEN_ONOFF_CLI, CID_NVAL);
    if (!onoff_cli_model) {
        LOGE(TAG, "find onoff model failed");
        return -EINVAL;
    }

    set_onoff_data.onoff = onoff;
    set_onoff_data.tid = 0;
    return ble_mesh_generic_onoff_set(0, 0, unicast_addr, onoff_cli_model, &set_onoff_data, 1);
}

int gateway_mesh_set_brightness(uint16_t unicast_addr, uint16_t bri)
{
    int                   ret        = 0;
    uint32_t              ack        = 0;
    uint32_t              appkey_idx = 0;
    struct bt_mesh_model *lightness_cli_model;

    lightness_cli_model = ble_mesh_model_find(0, BT_MESH_MODEL_ID_LIGHT_LIGHTNESS_CLI, CID_NVAL);
    if (!lightness_cli_model) {
        LOGE(TAG, "find brightness model failed");
        return -EINVAL;
    }

    set_lightness_data.range_min = 1;
    set_lightness_data.range_max = 255;

    aos_msleep(10);

    ret = ble_mesh_light_lightness_range_set(0, appkey_idx, unicast_addr, lightness_cli_model, &set_lightness_data,
                                             ack);
    if (ret != 0) {
        LOGE(TAG, "set bri failed on dev %d", unicast_addr);
    }

    ack                          = 1;
    set_lightness_data.lightness = bri;
    set_lightness_data.tid       = 0;
    ret = ble_mesh_light_lightness_set(0, appkey_idx, unicast_addr, lightness_cli_model, &set_lightness_data, ack);
    if (ret != 0) {
        LOGE(TAG, "set bri failed on dev %d", unicast_addr);
    }

    return ret;
}

int gateway_mesh_set_cct(uint16_t unicast_addr, uint16_t cct)
{
    int                   ret        = 0;
    uint32_t              ack        = 0;
    uint32_t              appkey_idx = 0;
    struct bt_mesh_model *light_ctl_model;

    light_ctl_model = ble_mesh_model_find(0, BT_MESH_MODEL_ID_LIGHT_CTL_CLI, CID_NVAL);
    if (!light_ctl_model) {
        return -EINVAL;
    }

    set_light_ctl_data.range_min = 2700;
    set_light_ctl_data.range_max = 6500;

    ret = ble_mesh_light_ctl_temp_range_set(0, appkey_idx, unicast_addr, light_ctl_model, &set_light_ctl_data, ack);
    if (ret != 0) {
        LOGE(TAG, "set bri failed on dev %d", unicast_addr);
    }

    ack                            = 1;
    set_light_ctl_data.temperature = cct;
    set_light_ctl_data.delta_uv    = 1;
    set_light_ctl_data.tid         = 0;

    aos_msleep(10);

    ret = ble_mesh_light_ctl_temp_set(0, appkey_idx, unicast_addr, light_ctl_model, &set_light_ctl_data, ack);
    if (ret != 0) {
        LOGE(TAG, "set bri failed on dev %d", unicast_addr);
    }

    return ret;
}

int gateway_mesh_get_onoff(uint16_t unicast_addr)
{
    struct bt_mesh_model *onoff_cli_model;

    onoff_cli_model = ble_mesh_model_find(0, BT_MESH_MODEL_ID_GEN_ONOFF_CLI, CID_NVAL);
    if (!onoff_cli_model) {
        return -EINVAL;
    }

    return ble_mesh_generic_onoff_get(0, 0, unicast_addr, onoff_cli_model);
}

int gateway_mesh_get_brightness(uint16_t unicast_addr)
{
    int                   ret        = 0;
    uint32_t              ack        = 0;
    uint32_t              appkey_idx = 0;
    struct bt_mesh_model *lightness_cli_model;

    lightness_cli_model = ble_mesh_model_find(0, BT_MESH_MODEL_ID_LIGHT_LIGHTNESS_CLI, CID_NVAL);

    if (!lightness_cli_model) {
        LOGE(TAG, "find brightness model failed");
        return -EINVAL;
    }

    set_lightness_data.range_min = 1;
    set_lightness_data.range_max = 100;

    ret = ble_mesh_light_lightness_range_set(0, appkey_idx, unicast_addr, lightness_cli_model, &set_lightness_data,
                                             ack);
    if (ret != 0) {
        LOGE(TAG, "set bri failed on dev %d", unicast_addr);
    }
    aos_msleep(10);

    ret = ble_mesh_light_lightness_get(0, appkey_idx, unicast_addr, lightness_cli_model);
    if (ret != 0) {
        LOGE(TAG, "set bri failed on dev %d", unicast_addr);
    }

    return ret;
}

int gateway_mesh_gw_set_onoff(uint8_t onoff)
{
    uint16_t              unicast_addr = 0;
    uint8_t               elem_num     = 0;
    struct bt_mesh_model *onoff_cli_model;

    bt_mesh_provisioner_get_own_unicast_addr(&unicast_addr, &elem_num);

    onoff_cli_model = ble_mesh_model_find(0, BT_MESH_MODEL_ID_GEN_ONOFF_CLI, CID_NVAL);
    if (!onoff_cli_model) {
        return -EINVAL;
    }

    set_onoff_data.onoff = onoff;
    set_onoff_data.tid = 0;

    return ble_mesh_generic_onoff_set(0, 0, unicast_addr, onoff_cli_model, &set_onoff_data, 1);
}

int gateway_mesh_send_rawdata(uint16_t unicast_addr, uint8_t *rawdata, int data_len)
{
    struct bt_mesh_model *vendor_model;
    vnd_model_msg         vnd_msg = { 0 };

    vendor_model = ble_mesh_model_find(0, BT_MESH_MODEL_VND_MODEL_CLI, BT_MESH_MODEL_VND_COMPANY_ID);
    if (!vendor_model) {
        LOGE(TAG, "found vendor model failed");
        return -EINVAL;
    }

    vnd_msg.netkey_idx = 0;
    vnd_msg.appkey_idx = 0;
    vnd_msg.dst_addr   = unicast_addr;
    vnd_msg.model      = vendor_model;
    vnd_msg.len        = data_len;
    vnd_msg.opid       = VENDOR_OP_ATTR_TRANS_MSG;
    vnd_msg.data       = rawdata;
#if defined(CONFIG_BT_MESH_EXT_ADV) && CONFIG_BT_MESH_EXT_ADV > 0
    vnd_msg.trans = NET_TRANS_EXT_ADV_2M;
#else
    vnd_msg.trans = NET_TRANS_LEGACY;
#endif
    return ble_mesh_vendor_cli_model_msg_send(&vnd_msg);
}

uint16_t gateway_mesh_get_subgrp_subaddr(gw_subgrp_t subgrp)
{
    return subgrp + GATEWAY_DEFAULT_SUB_ADDR;
}

int gateway_mesh_sub_add(uint16_t unicast_addr, uint16_t sub_addr)
{
    uint16_t mod_id = GATEWAY_DEFAULT_MOD_ID;
    return bt_mesh_cfg_mod_sub_add(0, unicast_addr, unicast_addr, sub_addr, mod_id, NULL);
}

int gateway_mesh_sub_del(uint16_t unicast_addr, uint16_t sub_addr)
{
    uint16_t mod_id = GATEWAY_DEFAULT_MOD_ID;
    return bt_mesh_cfg_mod_sub_del(0, unicast_addr, unicast_addr, sub_addr, mod_id, NULL);
}

int gateway_mesh_control_sub_onoff(uint16_t sub_addr, uint8_t onoff)
{
    struct bt_mesh_model *onoff_cli_model;
    int                   ret = 0;

    onoff_cli_model = ble_mesh_model_find(0, BT_MESH_MODEL_ID_GEN_ONOFF_CLI, CID_NVAL);
    if (!onoff_cli_model) {
        return -EINVAL;
    }

    set_onoff_data.onoff = onoff;
    set_onoff_data.tid = 0;

    ret = ble_mesh_generic_onoff_set(0, 0, sub_addr, onoff_cli_model, &set_onoff_data, 0);
    if (ret != 0) {
        LOGE(TAG, "set onoff failed on dev %d", sub_addr);
    }

    return ret;
}

int gateway_mesh_del_node(uint16_t unicast_addr)
{
    struct bt_mesh_node_t *node = NULL;
    int                    res  = 0;

    node = bt_mesh_provisioner_get_node_info(unicast_addr);
    if (!node) {
        LOGE(TAG, "subdev delete handle. found node fail");
        return -EINVAL;
    }

    bt_mesh_cfg_cli_timeout_set(0);
    res = bt_mesh_cfg_node_reset(0, unicast_addr);
    if (res) {
        LOGE(TAG, "subdev delete handle. send rst to %04x fail err %d", unicast_addr, res);
        return res;
    }
    bt_mesh_cfg_cli_timeout_set(2);

    return res;
}

int gateway_mesh_prov_config(uint32_t start_addr, uint32_t end_addr)
{
    int                       ret = 0;
    provisioner_node_config_t g_provisioner_param;
    g_provisioner_param.unicast_start_addr = start_addr;
    g_provisioner_param.unicast_end_addr   = end_addr;
    g_provisioner_param.attention_time     = GW_DEF_ATTENTION_TIMEOUT;
    ret                                    = ble_mesh_node_provisioner_config(&g_provisioner_param);
    if (ret != 0) {
        LOGE(TAG, "prov failed");
    }
    return ret;
}

int gateway_mesh_prov_autoconfig(void)
{
    return ble_mesh_node_provisioner_autoprov();
}

int gateway_mesh_prov_enable(uint8_t enable)
{
    int ret = 0;
    if (enable == 1) {
        ret = ble_mesh_node_provisioner_enable();
    } else {
        ret = ble_mesh_node_provisioner_disable();
    }

    return ret;
}

int gateway_mesh_prov_showdev(uint8_t show_dev, uint32_t timeout)
{
    uint32_t scan_timeout = GW_DEF_FOUND_DEV_TIMEOUT;
    if (timeout != GW_DEV_DISCOVER_NO_TIMEOUT) {
        scan_timeout = timeout;
    }
    return ble_mesh_provisioner_show_dev(show_dev, scan_timeout);
}

int gateway_mesh_prov_add_dev(gw_ble_mesh_info_t ctx)
{
    if (ctx.addr_type != DEV_ADDR_LE_RANDOM && ctx.addr_type != DEV_ADDR_LE_PUBLIC) {
        return -EINVAL;
    }

    if (ctx.bearer != BT_MESH_PROV_ADV && ctx.bearer != BT_MESH_PROV_GATT) {
        return -EINVAL;
    }

    int         ret             = 0;
    mesh_node_t node            = { 0x0 };
    uint32_t    auto_add_appkey = GW_DEF_AUTO_ADD_APPKEY_ONOFF;
    node.bearer                 = ctx.bearer;
    node.addr_type              = ctx.addr_type;
    node.oob_info               = ctx.oob_info;
    memcpy(node.dev_addr, ctx.dev_addr, 6);
    memcpy(node.uuid, ctx.uuid, 16);

    ret = ble_mesh_provisioner_dev_add(&node, auto_add_appkey);
    if (ret < 0 && ret != -EALREADY) {
        LOGE(TAG, "dev add fail");
        return ret;
    }
    return 0;
}

int gateway_mesh_prov_node_auto_config(uint16_t unicast_addr)
{
    model_auto_config_t auto_config;
    uint32_t            sub_addr = GW_AUTOCONFIG_DST_SUB_ADDR;
    int                 ret      = 0;

    auto_config.sub_addr = sub_addr;

    ret = ble_mesh_node_model_autoconfig(0, 0, unicast_addr, auto_config);
    if (ret != 0) {
        LOGE(TAG, "auto config fail on dev %d", unicast_addr);
    }

    return ret;
}
