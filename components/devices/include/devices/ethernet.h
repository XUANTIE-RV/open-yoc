 /*
 * Copyright (C) 2017-2024 Alibaba Group Holding Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _DRIVERS_ETHDRV_H_
#define _DRIVERS_ETHDRV_H_

#include <devices/device.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAC_SPEED_10M           1  ///< 10 Mbps link speed
#define MAC_SPEED_100M          2  ///< 100 Mbps link speed
#define MAC_SPEED_1G            3  ///< 1 Gpbs link speed

#define MAC_DUPLEX_HALF         1 ///< Half duplex link
#define MAC_DUPLEX_FULL         2 ///< Full duplex link

/** @brief mac control ethernet config */
typedef struct eth_config {
    int speed;
    int duplex;
    int loopback;
    uint8_t mac[6];
    void *net_pin;
} eth_config_t;

/**
  \brief       ethernet reset
  \param[in]   dev      Pointer to device object.
  \return      0 on success, else on fail.
*/
int rvm_hal_eth_reset(rvm_dev_t *dev);

/**
  \brief       ethernet start
  \param[in]   dev      Pointer to device object.
  \return      0 on success, else on fail.
*/
int rvm_hal_eth_start(rvm_dev_t *dev);

/**
  \brief       ethernet stop
  \param[in]   dev      Pointer to device object.
  \return      0 on success, else on fail.
*/
int rvm_hal_eth_stop(rvm_dev_t *dev);

/**
  \brief       ethernet set mac control config
  \param[in]   dev      Pointer to device object.
  \param[in]   config   mac control config.
  \return      0 on success, else on fail.
*/
int rvm_hal_eth_mac_control(rvm_dev_t *dev, eth_config_t *config);

/**
  \brief       ethernet set pakcet filter
  \param[in]   dev      Pointer to device object.
  \param[in]   type     packet filter type.
  \return      0 on success, else on fail.
*/
int rvm_hal_eth_set_packet_filter(rvm_dev_t *dev, int type);


#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/vfs_ethernet.h>
#endif

#ifdef __cplusplus
}
#endif

#endif
