/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */

#ifndef _APP_NETWORK_
#define _APP_NETWORK_

#include "app_wifi.h"

/*************
 * USER EVENT LIST
 ************/
/* 网络处理 */
#define EVENT_NTP_RETRY_TIMER            (EVENT_USER + 1)
#define EVENT_NET_CHECK_TIMER            (EVENT_USER + 2)
#define EVENT_NET_LPM_RECONNECT          (EVENT_USER + 4)
#define EVENT_NET_RECONNECT              (EVENT_USER + 5)

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
