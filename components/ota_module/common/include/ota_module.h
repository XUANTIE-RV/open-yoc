/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */
#ifndef __OTA_MODULE_H
#define __OTA_MODULE_H

typedef enum
{
    OTA_CHANNEL_ON_GATT = 0x00,
    OTA_CHANNEL_ON_UART,
    OTA_CHANNEL_ON_HCI_UART,
    OTA_CHANNEL_ON_MESH_EXT_1M,
    OTA_CHANNEL_ON_MESH_EXT_2M,
    OTA_CHANNEL_MAX,
} ota_channel_en;

#ifndef CONFIG_ALLOW_OTA_FOR_HIGH_VERSION
#define CONFIG_ALLOW_OTA_FOR_HIGH_VERSION 1
#endif

#endif
