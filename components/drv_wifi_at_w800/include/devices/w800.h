/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef DEVICE_ESP8266_H
#define DEVICE_ESP8266_H

#include <stdint.h>
#include <uservice/uservice.h>
#include <yoc/at_port.h>

#ifdef __cplusplus
extern "C" {
#endif

/* xxx_pin= 0 indicate the pin is not use */
typedef struct {
    int             reset_pin;
    int             cs_pin;
    int             wakeup_pin;
    int             int_pin;
    int             channel_id;
    int             baud;
    int             buffer_size;
    // at_channel_t   *channel;
} w800_wifi_param_t;

/**
 * This function will init atparser for w800
 * @param[in]   task         userver_tast
 * @param[in]   idx          uart_port
 * @param[in]   baud         uart_baud
 * @param[in]   buf_size     uart_rb_size
 * @param[in]   flow_control uart_flowcontrol
 * @return      Zero on success, -1 on failed
 */
int w800_module_init(utask_t *task, w800_wifi_param_t *param);

extern void wifi_w800_register(utask_t *task, w800_wifi_param_t *param);

#ifdef __cplusplus
}
#endif

#endif
