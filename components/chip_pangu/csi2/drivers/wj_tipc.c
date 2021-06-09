/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     tipc.c
 * @brief
 * @version
 * @date     27. April 2020
 ******************************************************************************/
#include <string.h>
#include <soc.h>
#include "wj_tipc_ll.h"
#include "sys_tipc.h"
#include "drv/tipc.h"
#include "drv/porting.h"

/**
  \brief       Config the tipc module properity
  \param[in]   dev dev handle \ref csi_dev_t
  \param[in]   is_secure is secure or not
*/

extern csi_tipcmap_t tipc_map[];
csi_error_t csi_dev_secure_config(csi_dev_t *dev, bool is_secure)
{
    csi_tipcmap_t *map = tipc_map;

    while (map->ip != 0xFFFFFFFFU) {
        if ((map->dev_tag == dev->dev_tag) &&
            (map->idx == dev->idx)) {
            if (is_secure) {
                soc_tipc_enable_secure(map->ip);
            } else {
                soc_tipc_disable_secure(map->ip);
            }

            break;
        }

        map++;
    }

    return CSI_OK;
}
