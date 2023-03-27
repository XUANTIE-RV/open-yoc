/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */
#ifndef __GATEWAY_MESH_MODEL_H__
#define __GATEWAY_MESH_MODEL_H__

#include "gateway.h"
#include <tsl_engine/file_utils.h>
#include <tsl_engine/device.h>
#include <tsl_engine/cloud_device_conv.h>
#include <tsl_engine/jse.h>
#include <tsl_engine/device_all.h>

typedef int (*gateway_model_send_data_t)(addr_t *addr, uint8_t *data, size_t size);
extern void gateway_model_conv_init(void);

#endif
