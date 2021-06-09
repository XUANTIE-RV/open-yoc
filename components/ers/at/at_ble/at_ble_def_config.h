/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#ifndef _AT_BLE_DEF_CONFIG_H
#define _AT_BLE_DEF_CONFIG_H


#define RF_PHY_TX_POWER_EXTRA_MAX                   0x3f
#define RF_PHY_TX_POWER_MAX                         0x1f
#define RF_PHY_TX_POWER_MIN                         0x00

#define RF_PHY_TX_POWER_5DBM                        0x17
#define RF_PHY_TX_POWER_4DBM                        0x12
#define RF_PHY_TX_POWER_3DBM                        0x0f
#define RF_PHY_TX_POWER_0DBM                        0x0a

#define RF_PHY_TX_POWER_N3DBM                       0x06
#define RF_PHY_TX_POWER_N5DBM                       0x05
#define RF_PHY_TX_POWER_N6DBM                       0x04

#define RF_PHY_TX_POWER_N10DBM                      0x03
#define RF_PHY_TX_POWER_N15DBM                      0x02
#define RF_PHY_TX_POWER_N20DBM                      0x01

enum ADV_DEF {
    ADV_OFF,
    ADV_ON,
};

enum CONN_DEF {
    CONN_AUTO_OFF,
    CONN_AUTO_ON,
};





#define SLAVE_DEVICE_NAME_DEF "YoC Slave"
#define MASTER_DEVICE_NAME_DEF "YoC Master"
#define DEF_ROLE   SLAVE
#define ADV_DEF_ON_OFF  ADV_ON
#define CONN_AUTO_CONF CONN_AUTO_ON
#define DEF_LPM_MODE SLEEP
#define CONN_DEF_ON 1
#define AD_DEF_TYPE ADV_IND
#define AD_INT_MIN_DEF ADV_FAST_INT_MIN_2
#define AD_INT_MAX_DEF ADV_FAST_INT_MIN_2
#define BT_TX_POWER_DEF RF_PHY_TX_POWER_0DBM
#define BT_TX_BAUD_DEF 115200
#define FLAG  (AD_FLAG_GENERAL | AD_FLAG_NO_BREDR)
#define AUTO_CONN_BT_MAX 3
#define RX_MAX_LEN 244
#define TX_MAX_LEN 244
#define CONN_UPDATE_DEF_ON_OFF 0
#define YOC_UART_SERVICE_UUID UUID128_DECLARE(0x7e,0x31,0x35,0xd4,0x12,0xf3,0x11,0xe9, 0xab,0x14,0xd6,0x63,0xbd,0x87,0x3d,0x93)

#define AD_INT_MIN_MIN 160
#define AD_INT_MIN_MAX 16384
#define AD_INT_MAX_MIN 160
#define AD_INT_MAX_MAX 16384


#endif
