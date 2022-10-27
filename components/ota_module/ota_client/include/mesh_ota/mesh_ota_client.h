/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef _MESH_OTA_CLIENT_H_
#define _MESH_OTA_CLIENT_H_

#define CONFIG_BT_MESH_OTA_REBOOT_DELAY     (5000) // ms
#define CONFIG_BT_MESH_OTA_REPORT_CRC_DELAY (2000) // ms
#define CONFIG_BT_MESH_OTA_REPORT_CRC_RETRY (20)

int mesh_ota_client_init();

#endif
