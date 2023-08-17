/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */
#ifndef _APP_CX_RECORD_H_
#define _APP_CX_RECORD_H_

#include <stdint.h>
#include <stdbool.h>
#include "cx/preview/preview_service.h"

int app_linkvisual_init(void);
bool app_linkvisual_isstreaming();
int app_record_init(void);
int app_preview_init(preview_mode_t mode);

#endif