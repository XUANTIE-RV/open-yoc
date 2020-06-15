/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef __BLE_MESH_CONFIG_H_
#define __BLE_MESH_CONFIG_H_

#ifdef CONFIG_BLE_MESH_PROV
#include "configs/ble_mesh_prov_config.h"
#else
#error "NO CONFIG FOR BLE"
#endif

#endif