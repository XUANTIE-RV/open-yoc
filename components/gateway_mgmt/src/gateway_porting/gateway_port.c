/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <stdint.h>

#include "common/log.h"
#include "gateway.h"
#include "gateway_mgmt.h"
#include "gateway_mesh.h"

#define TAG "GATEWAY_PORTING"

int gateway_porting_gw_hb_recv_enable(uint8_t enable)
{
    return gateway_mesh_hb_recv_enable(enable);
}

int gateway_porting_gw_discover(uint8_t enable, uint32_t timeout)
{
    if (enable) {
        return gateway_mesh_prov_showdev(1, timeout);
    } else {
        return gateway_mesh_prov_showdev(0, timeout);
    }
}

int gateway_porting_subdev_add(gw_discovered_dev_t subdev)
{
    int ret;
    switch (subdev.protocol) {
        case GW_NODE_BLE_MESH: {
            ret = gateway_mesh_prov_add_dev(subdev.protocol_info.ble_mesh_info);
            if (ret) {
                LOGE(TAG, "Mesh add dev failed %d", ret);
            }
        } break;
        default:
            LOGE(TAG, "Not support protocol %02x", subdev.protocol);
            return -EINVAL;
    }
    return ret;
}

int gateway_porting_subdev_del(gw_subdev_ctx_t *subdev_ctx)
{
    int ret = 0;
    switch (subdev_ctx->dev_type.protocol) {
        case GW_NODE_BLE_MESH: {
            ret = gateway_mesh_del_node(subdev_ctx->info.ble_mesh_info.unicast_addr);
            if (ret) {
                LOGE(TAG, "Mesh sub del failed %d", ret);
            }
        } break;
        default:
            return -EOPNOTSUPP;
    }
    return ret;
}

int gateway_porting_dev_simple_rst(uint8_t protocol, void *arg)
{
    int ret = 0;
    switch (protocol) {
        case GW_NODE_BLE_MESH: {
            ret = gateway_mesh_del_node(*(uint16_t *)arg);
            if (ret) {
                LOGE(TAG, "Mesh sub del failed %d", ret);
            }
        } break;
        default:
            return -EOPNOTSUPP;
    }
    return ret;
}

int gateway_porting_subdev_sub_add(gw_subdev_ctx_t *subdev_ctx, gw_subgrp_t subgrp)
{
    int ret = 0;
    switch (subdev_ctx->dev_type.protocol) {
        case GW_NODE_BLE_MESH: {
            ret = gateway_mesh_sub_add(subdev_ctx->info.ble_mesh_info.unicast_addr,
                                       gateway_mesh_get_subgrp_subaddr(subgrp));
            if (ret) {
                LOGE(TAG, "Subdev sub add failed %d", ret);
                return -EIO;
            }
        } break;
        default:
            LOGE(TAG, "Not Support protocol %02x", subdev_ctx->dev_type.protocol);
            return -EINVAL;
    }
    return ret;
}

int gateway_porting_subdev_sub_del(gw_subdev_ctx_t *subdev_ctx, gw_subgrp_t subgrp)
{
    int ret = 0;
    switch (subdev_ctx->dev_type.protocol) {
        case GW_NODE_BLE_MESH: {
            ret = gateway_mesh_sub_del(subdev_ctx->info.ble_mesh_info.unicast_addr,
                                       gateway_mesh_get_subgrp_subaddr(subgrp));
            if (ret) {
                LOGE(TAG, "Mesh sub del failed %d", ret);
            }
        } break;
        default:
            return -EOPNOTSUPP;
    }
    return ret;
}

int gateway_porting_subdev_get_triples(gw_subdev_ctx_t *subdev_ctx, uint8_t triples_flag)
{
    int ret = 0;
    switch (subdev_ctx->dev_type.protocol) {
        case GW_NODE_BLE_MESH: {
            ret = gateway_mesh_node_triples_get(subdev_ctx->info.ble_mesh_info.unicast_addr, triples_flag);
            if (ret) {
                LOGE(TAG, "Mesh get triples failed %d", ret);
            }
        } break;
        default:
            return -EOPNOTSUPP;
    }
    return ret;
}

int gateway_porting_subdev_set_onoff(gw_subdev_ctx_t *subdev_ctx, uint8_t onoff)
{
    int ret = 0;
    switch (subdev_ctx->dev_type.protocol) {
        case GW_NODE_BLE_MESH: {
            ret = gateway_mesh_set_onoff(subdev_ctx->info.ble_mesh_info.unicast_addr, onoff);
            if (ret) {
                LOGE(TAG, "Mesh set onoff failed %d", ret);
            }
        } break;
        default:
            return -EOPNOTSUPP;
    }
    return ret;
}

int gateway_porting_subdev_set_brightness(gw_subdev_ctx_t *subdev_ctx, uint16_t brightness)
{
    int ret = 0;
    switch (subdev_ctx->dev_type.protocol) {
        case GW_NODE_BLE_MESH: {
            ret = gateway_mesh_set_brightness(subdev_ctx->info.ble_mesh_info.unicast_addr, brightness);
            if (ret) {
                LOGE(TAG, "Mesh set brightness failed %d", ret);
                return ret;
            }
        } break;
        default:
            return -EOPNOTSUPP;
    }
    return ret;
}

int gateway_porting_subdev_send_rawdata(gw_subdev_ctx_t *subdev_ctx, uint8_t *raw_data, int data_len)
{
    int ret = 0;

    switch (subdev_ctx->dev_type.protocol) {
        case GW_NODE_BLE_MESH: {
            ret = gateway_mesh_send_rawdata(subdev_ctx->info.ble_mesh_info.unicast_addr, raw_data, data_len);
            if (ret) {
                LOGE(TAG, "Mesh send rawdata failed %d", ret);
                return ret;
            }
        } break;
        default:
            return -EOPNOTSUPP;
    }
    return ret;
}

int gateway_porting_subdev_get_onoff(gw_subdev_ctx_t *subdev_ctx)
{
    int ret = 0;

    switch (subdev_ctx->dev_type.protocol) {
        case GW_NODE_BLE_MESH: {
            ret = gateway_mesh_get_onoff(subdev_ctx->info.ble_mesh_info.unicast_addr);
            if (ret) {
                LOGE(TAG, "Mesh get onoff failed %d", ret);
                return ret;
            }
        } break;
        default:
            return -EOPNOTSUPP;
    }
    return ret;
}

int gateway_porting_subdev_get_brightness(gw_subdev_ctx_t *subdev_ctx)
{
    int ret = 0;
    switch (subdev_ctx->dev_type.protocol) {
        case GW_NODE_BLE_MESH: {
            ret = gateway_mesh_get_brightness(subdev_ctx->info.ble_mesh_info.unicast_addr);
            if (ret) {
                LOGE(TAG, "Mesh get brightness failed %d", ret);
                return ret;
            }
        } break;
        default:
            return -EOPNOTSUPP;
    }

    return ret;
}

int gateway_porting_subdev_heartbeat_period_set(gw_subdev_ctx_t *subdev_ctx, uint8_t enable, uint16_t period_s)
{
    int ret = 0;
    // disable node pub
    switch (subdev_ctx->dev_type.protocol) {
        case GW_NODE_BLE_MESH: {
            ret = gateway_mesh_hb_pub_set(subdev_ctx->info.ble_mesh_info.unicast_addr, enable, period_s);
        } break;
        default:
            LOGE(TAG, "Unsport protocal %d for hb pub set", subdev_ctx->dev_type.protocol);
            return -EINVAL;
    }

    if (ret) {
        LOGE(TAG, "hb pub %s failed %d", enable ? "enable" : "disable", ret);
    }
    return ret;
}

int gateway_porting_subgrp_set_onoff(gw_subdev_protocol_e protocal,gw_subgrp_t subgrp, uint8_t onoff)
{
    int ret = 0;

    switch (protocal) {
    case GW_NODE_BLE_MESH: {
        ret = gateway_mesh_set_onoff(gateway_mesh_get_subgrp_subaddr(subgrp), onoff);
        if (ret) {
            LOGE(TAG, "Mesh set onoff failed %d", ret);
        }
    } break;

    default:
        return -EOPNOTSUPP;
    }

return ret;

}

int gateway_porting_subgrp_set_brightness(gw_subdev_protocol_e protocal,gw_subgrp_t subgrp, uint16_t brightness)
{
    int ret = 0;

    switch (protocal) {
    case GW_NODE_BLE_MESH: {
        ret = gateway_mesh_set_brightness(gateway_mesh_get_subgrp_subaddr(subgrp), brightness);
        if (ret) {
            LOGE(TAG, "Mesh set brightness failed %d", ret);
        }

    } break;

    default:
        return -EOPNOTSUPP;
    }
    return ret;
}
