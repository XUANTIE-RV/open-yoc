/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */
#ifndef _GATEWAY_FOTA_H_
#define _GATEWAY_FOTA_H_

#include <gateway.h>

typedef struct {
    uint8_t type;
    uint8_t val[6];
} device_mac_t;

void app_fota_init(void);
void app_fota_start(void);

#endif

