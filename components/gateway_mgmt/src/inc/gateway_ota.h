/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */
#ifndef _GATEWAY_OTA_H_
#define _GATEWAY_OTA_H_

#include "gateway.h"
#include "ota_server.h"

#define OTA_NOT_SUPPORT 0x00

#ifndef CONFIG_GATEWAY_FIRMWARE_MAX_NUM
#define CONFIG_GATEWAY_FIRMWARE_MAX_NUM (10)
#endif

typedef gw_subdev_status_cb_t gw_subdev_ota_cb_t;
typedef gw_event_cb_t         gw_ota_firmware_cb_t;

typedef struct {
    gw_ota_firmware_cb_t subdev_ota_cb;
    gw_subdev_ota_cb_t   firmware_use_cb;
} gw_ota_cb_t;

enum
{
    OTA_FOR_GATEWAY  = 0x0,
    OTA_FOR_BLE_NODE = 0x01,
};

typedef enum
{
    GW_OTA_AIS_BY_GATT     = 0x00,
    GW_OTA_AIS_BY_UART     = 0x01,
    GW_OTA_AIS_BY_HCI_UART = 0x02,
    GW_OTA_MESH_BY_EXT_1M  = 0x03,
    GW_OTA_MESH_BY_EXT_2M  = 0x04,
} gw_ota_type_en;

typedef void *gateway_ota_image_t;

int gateway_ota_init(gw_ota_cb_t *ota_cb);
void gateway_ota_mgmt_cb_register(gw_subdev_status_cb_t cb);

#endif
