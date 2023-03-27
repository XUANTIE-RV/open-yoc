/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdint.h>

#include <aos/aos.h>
#include <ulog/ulog.h>
#include "ota_server.h"
#include "ota_process.h"
#include "dfu_port.h"
#include "gateway.h"
#include "gateway_mgmt.h"
#include "gateway_ota.h"
#include "gateway_settings.h"

#define TAG "GW_OTA"

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
#endif

typedef struct {
    uint8_t status; /*0 - idle 1 in use*/
    uint8_t firmware_id;
} gw_ota_firmare_t;

gw_ota_firmare_t     g_firmwares[CONFIG_GATEWAY_FIRMWARE_MAX_NUM];
gw_ota_firmware_cb_t g_firmware_use_cb = NULL;
gw_subdev_ota_cb_t   g_subdev_ota_cb   = NULL;

static int firmware_get(uint8_t firmware_idx)
{
    for (int i = 0; i < ARRAY_SIZE(g_firmwares); i++) {
        if (g_firmwares[i].status && g_firmwares[i].firmware_id == firmware_idx) {
            return 0;
        }
    }
    LOGD(TAG, "Firmware %d not found", firmware_idx);
    return -1;
}

static int firmware_add(uint8_t firmware_idx)
{
    if (!firmware_get(firmware_idx)) {
        return 0;
    }
    for (int i = 0; i < ARRAY_SIZE(g_firmwares); i++) {
        if (!g_firmwares[i].status) {
            g_firmwares[i].firmware_id = firmware_idx;
            g_firmwares[i].status      = 1;
            return 0;
        }
    }
    return -1;
}

static int firmware_del(uint8_t firmware_idx)
{
    for (int i = 0; i < ARRAY_SIZE(g_firmwares); i++) {
        if (g_firmwares[i].status && g_firmwares[i].firmware_id == firmware_idx) {
            g_firmwares[i].firmware_id = 0;
            g_firmwares[i].status      = 0;
            return ota_server_upgrade_firmware_rm(firmware_idx);
        }
    }
    LOGD(TAG, "Firmware %d not found", firmware_idx);
    return 0;
}

static void _gateway_ota_firmware_event_cb(ota_firmware_state_en event, void *data)
{
    int                      ret        = 0;
    ota_firmware_state_data *event_data = (ota_firmware_state_data *)data;
    gw_evt_ota_firmware_t    fw_event;

    switch (event) {
        case FIRMWARE_STATE_IN_USE: {
            LOGD(TAG, "OTA firmware index :%02x in use", event_data->firmware_index);
            fw_event.index  = event_data->firmware_index;
            fw_event.status = GW_FIRMWARE_STATE_IN_USE;
        } break;
        case FIRMWARE_STATE_END_USE: {
            LOGD(TAG, "OTA firmware index :%02x end use, Remove it auto", event_data->firmware_index);
            fw_event.index  = event_data->firmware_index;
            fw_event.status = FIRMWARE_STATE_END_USE;
            ret             = firmware_del(fw_event.index);
            if (ret) {
                LOGE(TAG, "Firmware %d del failed %d", fw_event.index, ret);
            }
        } break;
        case FIRMWARE_STATE_REMOVED: {
            LOGD(TAG, "OTA firmware index :%02x removed", event_data->firmware_index);
            fw_event.index  = event_data->firmware_index;
            fw_event.status = FIRMWARE_STATE_REMOVED;
        } break;
        default:
            break;
    }

    if (g_firmware_use_cb) {
        g_firmware_use_cb(GW_SUBDEV_EVT_OTA_FW, (gw_evt_param_t)&fw_event);
    }
}

static int get_ota_protocol_and_channel(gw_subdev_t *subdev_list, uint8_t subdev_num, uint8_t *protocol,
                                        uint8_t *channel)
{
    uint8_t ais_gatt_num    = 0;
    uint8_t mesh_ext_1M_num = 0;
    uint8_t mesh_ext_2M_num = 0;

    for (int i = 0; i < subdev_num; i++) {
        gw_subdev_ctx_t *ctx_node = gateway_subdev_get_ctx(subdev_list[i]);
        if (!ctx_node) {
            LOGE(TAG, "No subdev found for ota");
            return -EINVAL;
        }
        if (ctx_node->support_ota_protocol == OTA_NOT_SUPPORT) {
            LOGE(TAG, "No support ota protocol for subdev %d", subdev_list[i]);
            return -EINVAL;
        }

        if (ctx_node->support_ota_protocol & (1 << GW_OTA_AIS_BY_GATT)) {
            ais_gatt_num++;
        }
        if (ctx_node->support_ota_protocol & (1 << GW_OTA_MESH_BY_EXT_1M)) {
            mesh_ext_1M_num++;
        }
        if (ctx_node->support_ota_protocol & (1 << GW_OTA_MESH_BY_EXT_2M)) {
            mesh_ext_2M_num++;
        }
    }

#if defined(CONFIG_BT_MESH_MODEL_BLOB_CLI) && CONFIG_BT_MESH_MODEL_BLOB_CLI > 0 && defined(CONFIG_BT_MESH_EXT_ADV)     \
    && CONFIG_BT_MESH_EXT_ADV > 0
    if (mesh_ext_2M_num == subdev_num) {
        LOGD(TAG, "All OTA node support Mesh ota 2M, use it Preferentially");
        *protocol = OTA_PROTOCOL_MESH;
        *channel  = OTA_CHANNEL_ON_MESH_EXT_2M;
        return 0;
    } else {
        LOGD(TAG, "Not all OTA node support Mesh ota 2M, will not use it");
    }

    if (mesh_ext_1M_num == subdev_num) {
        LOGD(TAG, "All OTA node support Mesh ota 1M, use it Secondly");
        *protocol = OTA_PROTOCOL_MESH;
        *channel  = OTA_CHANNEL_ON_MESH_EXT_1M;
        return 0;
    } else {
        LOGD(TAG, "Not all OTA node support Mesh ota 2M, will not use it");
    }
#endif

    if (ais_gatt_num == subdev_num) {
        LOGD(TAG, "All OTA node support Ais Gatt, use it");
        *protocol = OTA_PROTOCOL_AIS;
        *channel  = OTA_CHANNEL_ON_GATT;
        return 0;
    } else {
        LOGD(TAG, "Not all OTA node support Ais gatt, will not use it");
    }

    LOGE(TAG, "No OTA protocol support all the ota subdev list, Please check it");
    return -EINVAL;
}

static void _gateway_ota_dev_event_cb(ota_device_state_en event_id, void *param)
{
    gw_status_ota_t  dev_ota_status;
    gw_subdev_ctx_t *subdev_ctx = NULL;

    switch (event_id) {
        case OTA_STATE_ONGOING: {
            ota_state_ongoing *status = (ota_state_ongoing *)param;
            LOGI(TAG, "ota ready for firmware:%d by channel :%02x", status->firmware_index, status->channel);

            subdev_ctx = gateway_mgmt_get_subdev_ctx_by_mac((char *)status->dev_info.addr.val);
            if (subdev_ctx) {
                dev_ota_status.subdev         = subdev_ctx->subdev;
                dev_ota_status.status         = GW_OTA_STATE_ONGOING;
                dev_ota_status.firmware_index = status->firmware_index;
                dev_ota_status.old_version    = subdev_ctx->version;
                dev_ota_status.new_version    = subdev_ctx->version;
                dev_ota_status.cost_time      = 0;
            } else {
                LOGE(TAG, "Get subdev info failed");
                return;
            }
        } break;

        case OTA_STATE_SUCCESS: {
            ota_state_success *status = (ota_state_success *)param;

            LOGI(TAG,
                 "ota success for firmware:%d unicast_addr:0x%04x dev: %02x:%02x:%02x:%02x:%02x:%02x type: %d "
                 "old_version:0x%04x new_verison:0x%04x cost %d ms by channel :%02x",
                 status->firmware_index, status->dev_info.unicast_addr, status->dev_info.addr.val[5],
                 status->dev_info.addr.val[4], status->dev_info.addr.val[3], status->dev_info.addr.val[2],
                 status->dev_info.addr.val[1], status->dev_info.addr.val[0], status->dev_info.addr.type,
                 status->old_ver, status->new_ver, status->cost_time, status->channel);

            subdev_ctx = gateway_mgmt_get_subdev_ctx_by_mac((char *)status->dev_info.addr.val);
            if (subdev_ctx) {
                if (subdev_ctx->version != status->new_ver) {

                    subdev_ctx->version = status->new_ver;
                    gateway_subdev_ctx_store(subdev_ctx);
                }
                dev_ota_status.subdev         = subdev_ctx->subdev;
                dev_ota_status.status         = GW_OTA_STATE_SUCCESS;
                dev_ota_status.firmware_index = status->firmware_index;
                dev_ota_status.old_version    = status->old_ver;
                dev_ota_status.new_version    = status->new_ver;
                dev_ota_status.cost_time      = status->cost_time;
            } else {
                LOGE(TAG, "Get subdev info failed");
                return;
            }
        } break;

        case OTA_STATE_FAILED: {
            ota_state_fail *status = (ota_state_fail *)param;
            LOGE(TAG,
                 "ota fail for firmware:%d unicast_addr:0x%04x dev: %02x:%02x:%02x:%02x:%02x:%02x type: %d fail "
                 "reason:%s by channel :%02x",
                 status->firmware_index, status->dev_info.unicast_addr, status->dev_info.addr.val[5],
                 status->dev_info.addr.val[4], status->dev_info.addr.val[3], status->dev_info.addr.val[2],
                 status->dev_info.addr.val[1], status->dev_info.addr.val[0], status->dev_info.addr.type,
                 ota_fail_str(status->reason), status->channel);
            subdev_ctx = gateway_mgmt_get_subdev_ctx_by_mac((char *)status->dev_info.addr.val);
            if (subdev_ctx) {
                if (status->reason == OTA_FAIL_INVAILD_VERSION && subdev_ctx->version != status->old_ver) {
                    subdev_ctx->version = status->old_ver;
                    gateway_subdev_ctx_store(subdev_ctx);
                }
                dev_ota_status.subdev         = subdev_ctx->subdev;
                dev_ota_status.status         = GW_OTA_STATE_FAILED;
                dev_ota_status.firmware_index = status->firmware_index;
                dev_ota_status.old_version    = status->old_ver;
                dev_ota_status.new_version    = status->new_ver;
                dev_ota_status.cost_time      = status->cost_time;
            } else {
                LOGE(TAG, "Get subdev info failed");
                return;
            }
        } break;
        default:
            LOGE(TAG, "Unkonw ota event %02x", event_id);
            return;
    }
    if (g_subdev_ota_cb) {
        g_subdev_ota_cb(subdev_ctx->subdev, GW_SUBDEV_STATUS_OTA, (void *)&dev_ota_status, sizeof(gw_status_ota_t));
    }
}

static struct ota_server_cb gateway_ota_cb = {
    .device_event_cb = _gateway_ota_dev_event_cb,
};

static int gateway_ota_node_upgrade(const void *image, uint32_t size, uint8_t protocol, uint8_t channel,
                                    gw_subdev_t *subdev_list, uint8_t subdev_num)
{
    int ret         = 0;
    int firmware_id = -1;

    firmware_info    info;
    device_info *    ota_devices = NULL;
    gw_subdev_ctx_t *subdev_info = NULL;

    if (!image || !size) {
        LOGE(TAG, "Invalid image info");
        return -EINVAL;
    }

    if (!(subdev_list || subdev_num)) {
        LOGE(TAG, "Invalid ota subdev info");
        return -EINVAL;
    }

    info.address    = (uint8_t *)image;
    info.image_type = 0x00;
    info.image_pos  = IMAGE_POS_RAM;
    info.size       = dfu_get_fota_file_size(info.address, info.image_pos);
    info.version    = dfu_get_fota_file_app_version(info.address, info.image_pos);
    info.ota_chanel = channel;
    info.protocol   = protocol;
    info.cb         = _gateway_ota_firmware_event_cb;

    if (info.size < 0) {
        LOGE(TAG, "read fota image fail\r\n");
        return -1;
    } else {
        LOGI(TAG, "fota image size %d\r\n", info.size);
    }

    firmware_id = ota_server_upgrade_firmware_add(info);
    if (firmware_id < 0) {
        LOGE(TAG, "Add OTA firmware failed");
        return -EBADF;
    }

    ota_devices = (device_info *)aos_zalloc(subdev_num);

    if (!ota_devices) {
        LOGE(TAG, "Ota devices alloc failed");
        return -ENOMEM;
    }

    for (int i = 0; i < subdev_num; i++) {
        subdev_info = gateway_subdev_get_ctx(subdev_list[i]);
        if (subdev_info) {
            memcpy(ota_devices[i].addr.val, subdev_info->info.ble_mesh_info.dev_addr, 6);
			ota_devices[i].addr.type   = subdev_info->info.ble_mesh_info.addr_type;
            ota_devices[i].old_version = subdev_info->version;
			ota_devices[i].unicast_addr = subdev_info->info.ble_mesh_info.unicast_addr;
        } else {
            LOGE(TAG, "Get subdev index %d info failed", subdev_list[i]);
            aos_free(ota_devices);
            return -EINVAL;
        }
    }

    ret = ota_server_upgrade_device_add(firmware_id, subdev_num, ota_devices);
    if (ret) {
        firmware_id = -1;
        LOGE(TAG, "add ota dev failed");
        aos_free(ota_devices);
        return ret;
    }

    aos_free(ota_devices);
    if (protocol == OTA_PROTOCOL_MESH) {
        LOGD(TAG, "You should call ota start api after add all ota node when using mesh protocol");
    }

    return firmware_id;
}

static int gateway_ota_node_del(int fw_index, gw_subdev_t *subdev_list, uint8_t subdev_num)
{
    int          ret         = 0;
    device_info *ota_devices = NULL;

    if (!(subdev_list || subdev_num)) {
        LOGE(TAG, "Invalid ota subdev info");
        return -EINVAL;
    }

    ota_devices = (device_info *)aos_zalloc(subdev_num);
    if (!ota_devices) {
        LOGE(TAG, "Ota devices alloc failed");
        return -1;
    }

    ret = ota_server_upgrade_device_rm(fw_index, subdev_num, ota_devices);
    if (ret) {
        LOGE(TAG, "rm ota dev failed");
        aos_free(ota_devices);
        return -1;
    }

    aos_free(ota_devices);

    return 0;
}

/* user api*/
int gateway_ota_init(gw_ota_cb_t *ota_cb)
{
    int err = 0;
#if defined(CONFIG_OTA_SERVER)
    if (!ota_cb->subdev_ota_cb || !ota_cb->firmware_use_cb) {
        LOGE(TAG, "gateway ota server cb not register");
        return -EINVAL;
    }

    err = ota_server_init();
    if (err && err != -EALREADY) {
        LOGE(TAG, "gateway ota server init failed %d", err);
        return err;
    }

    ota_server_cb_register(&gateway_ota_cb);
#else
    return -EINVAL;
#endif
    return 0;
}

void gateway_ota_mgmt_cb_register(gw_subdev_status_cb_t cb)
{
    g_subdev_ota_cb = cb;
}


gw_status_t gateway_subdev_ota(int *fw_index, gw_subdev_t *subdev_list, uint8_t subdev_num, const void *image,
                               uint32_t image_size)
{

    int     ret          = 0;
    int     firmware_idx = 0;
    uint8_t protocol     = 0;
    uint8_t channel      = 0;

    ret = get_ota_protocol_and_channel(subdev_list, subdev_num, &protocol, &channel);
    if (ret) {
        return ret;
    }

    firmware_idx = gateway_ota_node_upgrade(image, image_size, protocol, channel, subdev_list, subdev_num);
    if (firmware_idx >= 0) {
        ret = firmware_add(firmware_idx);
        if (ret) {
            LOGE(TAG, "Firmware %d add failed %d", firmware_idx, ret);
        }
        *fw_index = firmware_idx;
    } else {
        return firmware_idx;
    }

    return 0;
}

gw_status_t gateway_subdev_ota_stop(int fw_index, gw_subdev_t *subdev_list, uint8_t subdev_num)
{
    return gateway_ota_node_del(fw_index, subdev_list, subdev_num);
}
