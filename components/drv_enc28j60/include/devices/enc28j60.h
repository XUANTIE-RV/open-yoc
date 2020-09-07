/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef _DRIVERS_ENC28J60_H_
#define _DRIVERS_ENC28J60_H_

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include <aos/aos.h>

#include <devices/hal/ethernet_impl.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAC_SPEED_10M           1  ///< 10 Mbps link speed
#define MAC_SPEED_100M          2  ///< 100 Mbps link speed
#define MAC_SPEED_1G            3  ///< 1 Gpbs link speed

#define MAC_DUPLEX_HALF         1 ///< Half duplex link
#define MAC_DUPLEX_FULL         2 ///< Full duplex link

typedef struct enc28j60_pin {
    int enc28j60_spi_idx;
    int enc28j60_spi_rst;
    int enc28j60_spi_cs;
    int enc28j60_spi_interrupt;
} enc28j60_pin_t;


extern void eth_enc28j60_register(eth_config_t *eth_config);

#ifdef __cplusplus
}
#endif

#endif
