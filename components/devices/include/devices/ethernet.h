/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
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
