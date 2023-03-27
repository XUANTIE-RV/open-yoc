/*
 * Copyright (C) 2015-2022 Alibaba Group Holding Limited
 */
#include "stdio.h"
#include <aos/kernel.h>
#include <aos/debug.h>
#include <ulog/ulog.h>
#include <k_api.h>
#include "app_main.h"
#include "smartliving/exports/iot_export_errno.h"
#include "smartliving/exports/iot_export_ota.h"
#include "app_gateway_ut.h"

#define TAG "SLOTA"

#if defined(CONFIG_SMARTLIVING_OTA) && CONFIG_SMARTLIVING_OTA

static int      g_smartliving_image_idx    = -1;
static int      g_ota_from_smartliving     = 0;
static char *   smartliving_ota_image      = NULL;
static uint32_t smartliving_ota_image_size = 0;
static int      g_ota_for_dev_type         = GW_OTA_TARGET_LOCAL;

static char *g_gateway_subdev_ota_map = NULL;

extern int                     g_ota_for_dev_type;
extern char                    ble_node_version[128];
extern gw_subdev_cloud_info_t *g_gateway_subdev_cloud_info;
extern int                     dm_ota_get_ota_handle(void **handle);
extern int                     get_version_str(uint32_t version, char *ver_str);
extern int                     board_ota_gateway_upgrade(const void *image, uint32_t size);

static uint8_t _ota_node_bitmap_clear(gw_subdev_t subdev, void *data)
{
    int16_t       index       = subdev;
    subdev_info_t subdev_info = { 0 };
    gw_status_t   ret         = 0;
    addr_t *      mac         = (uint8_t *)data;

    ret = gateway_subdev_get_info(subdev, &subdev_info);
    if (ret != 0) {
        LOGE(TAG, "%s, get subdev_info fail", __FUNCTION__);
        return GW_SUBDEV_ITER_CONTINUE;
    }

    if (memcmp(subdev_info.dev_addr, mac, GW_DEV_ADDR_LEN) == 0) {
        g_gateway_subdev_ota_map[index] = 0;
    }

    return GW_SUBDEV_ITER_CONTINUE;
}

static int _smartliving_fota_version_rpt(char *version)
{
    void *ota_handle = NULL;
    int   res        = 0;
    /* Get Ota Handle */
    res = dm_ota_get_ota_handle(&ota_handle);

    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    IOT_OTA_ReportVersion(ota_handle, version);

    return SUCCESS_RETURN;
}

static int _smartliving_node_upgrade_check(uint8_t mac[6], uint32_t version)
{
    int     ret                = 0;
    uint8_t ble_nodes_finished = 1;

    gateway_subdev_foreach(_ota_node_bitmap_clear, (void *)mac);

    for (int i = 1; i < MAX_DEVICES_META_NUM + 1; i++) {
        if (g_gateway_subdev_ota_map[i] == 1) {
            ble_nodes_finished = 0;
            break;
        }
    }

    if (ble_nodes_finished == 1) {
        if (g_ota_from_smartliving) { /* remove the firmware directly when firmware from smartliving */
            g_ota_from_smartliving = 0;
        }

        _smartliving_fota_version_rpt(ble_node_version);
    }
    return ret;
}

static int _add_ota_node(char *node_image, int image_size)
{
    if (!node_image) {
        return -1;
    }

    int          ret         = 0;
    uint8_t      ota_num     = 0;
    int          fw_index    = 0;
    gw_subdev_t *subdev_list = aos_zalloc(MAX_DEVICES_META_NUM);

    for (int i = 1; i < MAX_DEVICES_META_NUM + 1; i++) {
        if (g_gateway_subdev_ota_map[i] == 1) {
            subdev_list[ota_num] = i;
            ota_num++;
        }
    }

    ret = gateway_subdev_ota(&fw_index, subdev_list, ota_num, node_image, image_size);
    if (ret) {
        aos_free(subdev_list);
        LOGE(TAG, "subdev ota failed %d", ret);
        return ret;
    }

    aos_free(subdev_list);

    g_smartliving_image_idx = fw_index;

    return ret;
}

static void _smartliving_ota_dev_event_cb(gw_subdev_ota_state_e event_id, void *param)
{
    if (!smartliving_ota_image) {
        return;
    }

    switch (event_id) {
        case GW_OTA_STATE_ONGOING: {

        } break;

        case GW_OTA_STATE_SUCCESS: {
            gw_status_ota_t *status          = (gw_status_ota_t *)param;
            gw_subdev_t      subdev          = status->subdev;
            subdev_info_t    subdev_info     = { 0 };

            gateway_subdev_get_info(subdev, &subdev_info);

            _smartliving_node_upgrade_check(subdev_info.dev_addr, status->new_version);
        } break;

        case GW_OTA_STATE_FAILED: {

        } break;

        default:
            break;
    }
}

gw_status_t app_smartliving_ota_event_process(gw_event_type_e gw_evt, gw_evt_param_t gw_evt_param)
{
    switch (gw_evt) {
        case GW_SUBDEV_EVT_OTA_FW: {
            gw_evt_ota_firmware_t *gw_evt_ota = (gw_evt_ota_firmware_t *)gw_evt_param;

            if (g_smartliving_image_idx != gw_evt_ota->index) {
                return 0;
            }

            if (gw_evt_ota->status == GW_FIRMWARE_STATE_END_USE) {
                LOGD(TAG, "OTA firmware index :%02x end use", gw_evt_ota->index);
            } else if (gw_evt_ota->status == GW_FIRMWARE_STATE_REMOVED) {
                if (smartliving_ota_image) {
                    free(smartliving_ota_image);
                    g_smartliving_image_idx = -1;
                    smartliving_ota_image   = NULL;
                }
                LOGD(TAG, "OTA firmware index :%02x removed", gw_evt_ota->index);
            } else if (gw_evt_ota->status == GW_FIRMWARE_STATE_IN_USE) {
                LOGD(TAG, "OTA firmware index :%02x in use", gw_evt_ota->index);
            }

            break;
        }
        default:
            break;
    }

    return 0;
}

gw_status_t app_smartliving_ota_subdev_status_process(gw_subdev_t subdev, gw_subdev_status_type_e gw_status,
                                                      gw_status_param_t status, int status_len)
{
    switch (gw_status) {
        case GW_SUBDEV_STATUS_OTA: {
            gw_status_ota_t *ota_status = (gw_status_ota_t *)status;
            if (ota_status->firmware_index != g_smartliving_image_idx) {
                return 0;
            }
            _smartliving_ota_dev_event_cb((gw_subdev_ota_state_e)ota_status->status, (void *)ota_status);

            break;
        }
        default:
            break;
    }

    return 0;
}

void app_smartliving_ota_prepare(const char *version)
{
    char        sub_str[10]      = { 0 };
    char        ble_node_ver[10] = "ble_node";
    char        pk[32]           = { 0 };
    const char *str_ptr          = NULL;
    char *      tmp_ptr          = NULL;
    char        sub_ver[16]      = { 0 };
    uint8_t     pk_len           = 0;

    if (g_gateway_subdev_ota_map == NULL) {
        g_gateway_subdev_ota_map = aos_zalloc(MAX_DEVICES_META_NUM + 1);
        if (g_gateway_subdev_ota_map == NULL) {
            LOGE(TAG, "alloc g_gateway_subdev_ota_map fail!!!");
            return;
        }
    }

    strncpy(sub_str, version, strlen(ble_node_ver));
    LOGI(TAG, "sub_str %s", sub_str);

    if (strcmp(sub_str, ble_node_ver) == 0) {
        LOGI(TAG, "ota for ble node");

        str_ptr = version;
        str_ptr += (strlen(ble_node_ver) + 1);

        tmp_ptr = strchr(str_ptr, '_');
        pk_len  = tmp_ptr - str_ptr;
        LOGI(TAG, "len %d\r\n", pk_len);
        strncpy(pk, str_ptr, pk_len);
        LOGI(TAG, "pk %s\r\n", pk);

        str_ptr += (strlen(pk) + 1);
        strcpy(sub_ver, str_ptr);
        LOGI(TAG, "sub ver %s\r\n", sub_ver);

        for (uint16_t i = 1; i < MAX_DEVICES_META_NUM + 1; i++) {
            if (strcmp(pk, g_gateway_subdev_cloud_info->info[i].meta.product_key) == 0) {
                g_gateway_subdev_ota_map[i] = 1;
            }
        }

        g_ota_for_dev_type = GW_OTA_TARGET_REMOTE;

        strcpy(ble_node_version, version);

    } else {
        LOGI(TAG, "ota for gateway");

        g_ota_for_dev_type = GW_OTA_TARGET_LOCAL;
    }
}

void HAL_Firmware_Persistence_Start(uint32_t file_size)
{
    if (smartliving_ota_image == NULL) {
        smartliving_ota_image = aos_zalloc(CONFIG_OTA_GATEWAY_IMG_SIZE);

        if (smartliving_ota_image == NULL) {
            g_smartliving_image_idx = -1;
            LOGE(TAG, "no place for gateway image!!!");
        }
    }

#ifdef WIFI_PROVISION_ENABLED
    extern int awss_suc_notify_stop(void);
    awss_suc_notify_stop();
#endif
#ifdef DEV_BIND_ENABLED
    extern int awss_dev_bind_notify_stop(void);
    awss_dev_bind_notify_stop();
#endif
}

int HAL_Firmware_Persistence_Stop(void)
{
    void *   ota_handle   = NULL;
    int      res          = SUCCESS_RETURN;

    /* Get Ota Handle */
    res = dm_ota_get_ota_handle(&ota_handle);

    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    if (ota_handle == NULL) {
        return FAIL_RETURN;
    }

    if (IOT_OTA_IsFetchFinish(ota_handle) != 1) {
        LOGE(TAG, "ota fail since file fetch not finished");
        return FAIL_RETURN;
    }

    if (g_ota_for_dev_type == GW_OTA_TARGET_LOCAL) {
        g_ota_from_smartliving = 1;
        return board_ota_gateway_upgrade(smartliving_ota_image, smartliving_ota_image_size);
    } else if (g_ota_for_dev_type == GW_OTA_TARGET_REMOTE) {
        g_ota_from_smartliving = 1;
        return _add_ota_node(smartliving_ota_image, smartliving_ota_image_size);
    }

    return 0;
}

int HAL_Firmware_Persistence_Write(char *buffer, uint32_t length)
{
    memcpy(smartliving_ota_image + smartliving_ota_image_size, buffer, length);
    smartliving_ota_image_size += length;

    return 0;
}

#else
void app_smartliving_ota_prepare(const char *version)
{
    return;
}

void HAL_Firmware_Persistence_Start(void)
{
    return;
}

int HAL_Firmware_Persistence_Stop(void)
{
    return;
}

int HAL_Firmware_Persistence_Write(char *buffer, uint32_t length)
{
    return;
}

#endif
