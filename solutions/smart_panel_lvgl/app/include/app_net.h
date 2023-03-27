/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */

#ifndef _APP_NETWORK_
#define _APP_NETWORK_

#include "app_wifi.h"

/**
 * network init
 *
 * @return void
 */
void app_network_init(void);

/**
 * set network low power enable
 *
 * @param [in] lpm_en 1:can enter low power mode, 0: cannot enter low power mode
 * 
 * @return 1: connnecting
 */
void app_network_lpm_enable(int lpm_en);


/**
 * get network linkup status
 *
 * @return 1: connnecting
 */
int app_network_is_linkup();

/**
 * get internet connect status
 * 
 * @return 1: connected, 0: not connected
 */
int  app_network_internet_is_connected();

/**
 * set internet connect status
 * 
 * @return void
 */
void app_network_internet_set_connected(int connected);



#endif
