/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef _DRIVERS_ETHDRV_H_
#define _DRIVERS_ETHDRV_H_

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include <aos/aos.h>

#include <devices/hal/net_impl.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAC_SPEED_10M           1  ///< 10 Mbps link speed
#define MAC_SPEED_100M          2  ///< 100 Mbps link speed
#define MAC_SPEED_1G            3  ///< 1 Gpbs link speed

#define MAC_DUPLEX_HALF         1 ///< Half duplex link
#define MAC_DUPLEX_FULL         2 ///< Full duplex link

typedef struct eth_config {
    int speed;
    int duplex;
    int loopback;
    uint8_t mac[6];
    void *net_pin;
} eth_config_t;

typedef struct eth_driver {
    /*common*/
    int (*mac_control)(aos_dev_t *dev, eth_config_t *config);
    int (*set_packet_filter)(aos_dev_t *dev, int type);
    int (*start)(aos_dev_t *dev);
    int (*restart)(aos_dev_t *dev);
    int (*ping)(aos_dev_t *dev, int type, char *remote_ip);
    int (*ifconfig)(aos_dev_t *dev);
} eth_driver_t;

#ifdef __cplusplus
}
#endif

#endif
