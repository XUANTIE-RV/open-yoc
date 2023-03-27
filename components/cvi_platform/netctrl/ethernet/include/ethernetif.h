/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __ETHERNETIF_H__
#define __ETHERNETIF_H__

#include "lwip/err.h"
#include "lwip/netif.h"

err_t ethif_init(struct netif *netif);

#endif
