/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */
#ifndef _GATEWAY_FOTA_H_
#define _GATEWAY_FOTA_H_

#include <gateway.h>

#define APP_FOTA_SLEEP_TIME 12*60*60*1000

typedef struct {
    uint8_t type;
    uint8_t val[6];
} device_mac_t;

void app_fota_start(void);
void app_fota_rpt_gw_ver(void);
int  app_fota_is_downloading(void);
void app_fota_do_check(void);

int app_fota_init(void);
int app_fota_device_load();
int app_fota_device_add(const char *cid, const char *version, gw_subdev_t subdev);
int app_fota_device_delete(const char *cid);
int app_fota_device_show();
int app_fota_event_process(gw_event_type_e gw_evt, gw_evt_param_t gw_evt_param);
int app_fota_subdev_status_process(gw_subdev_t subdev, gw_subdev_status_type_e gw_status, gw_status_param_t status, int status_len);

#endif

