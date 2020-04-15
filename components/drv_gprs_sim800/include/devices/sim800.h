/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef DEVICE_SIM800_H
#define DEVICE_SIM800_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* xxx_pin= 0 indicate the pin is not use */
typedef struct {
    const char  *device_name;
    int         reset_pin;
    int         state_pin;
    uint32_t    baud;
    uint32_t    buf_size;
    uint8_t     enable_flowctl;
} sim_gprs_param_t;

/**
 * This function will init atparser for esp8266
 * @param[in]   task         userver_tast
 * @param[in]   idx          uart_port
 * @param[in]   baud         uart_baud
 * @param[in]   buf_size     uart_rb_size
 * @param[in]   flow_control uart_flowcontrol
 * @return      Zero on success, -1 on failed
 */
int sim800_module_init(utask_t *task, sim_gprs_param_t *param);

extern void gprs_sim800_register(utask_t *task, sim_gprs_param_t *param);

#ifdef __cplusplus
}
#endif

#endif
