/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#ifndef DEVICE_STL8723_H
#define DEVICE_STL8723_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct rtl8723ds_gpio_pin_t {
    int wl_en;
    int power;
} rtl8723ds_gpio_pin;

typedef struct rtl8723ds_bt_config_t {
    int uart_id;
    int bt_dis_pin;
} rtl8723ds_bt_config;

extern void wifi_rtl8723ds_register(rtl8723ds_gpio_pin* config);
extern void bt_rtl8723ds_register(rtl8723ds_bt_config* config);


#ifdef __cplusplus
}
#endif

#endif