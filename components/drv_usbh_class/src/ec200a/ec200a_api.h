/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef DEVICE_ec200a_API_H
#define DEVICE_ec200a_API_H

#include <devices/uart.h>
#include <devices/netdrv.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * This function will test wifi module is online/uart can use
 * @param[in]   NULL
 * @return      Zero on success, -1 on failed
 */
int ec200a_at0(void);

/**
 * This function will reset wifi module
 * @param[in]   NULL
 * @return      Zero on success, -1 on failed
 */
int ec200a_hard_reset(void);

/**
 * This function will init wifi module
 * @param[in]   NULL
 * @return      Zero on success, -1 on failed
 */
int ec200a_close_echo(void);

int ec200a_check_simcard_is_insert(uint8_t isAsync);

int ec200a_check_signal_quality(rvm_hal_sig_qual_resp_t *respond, uint8_t isAsync);

int ec200a_check_register_status(rvm_hal_regs_stat_resp_t *respond, uint8_t isAsync);

int ec200a_connect_to_network(uint8_t isAsync);

int ec200a_get_state_pin(void);

int ec200a_get_ccid(char ccid[21]);

void ec200a_set_timeout(int ms);

void ec200a_start_network(void);

int ec200a_module_init(char *deveice_name);

void ec200a_lwip_init(void);

int ec200a_check_net_type(int *net_type);

int ec200a_set_net_type(int net_type);
#ifdef __cplusplus
}
#endif

#endif
