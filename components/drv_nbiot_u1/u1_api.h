/*
 * Copyright (C) 2020 FishSemi Inc. All rights reserved.
 */

#ifndef DEVICE_U1_API_H
#define DEVICE_U1_API_H

#include <devices/uart.h>
#include <devices/netdrv.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    NBIOT_STATUS_NOREG = 0,
    NBIOT_STATUS_REGISTERED,
    NBIOT_STATUS_REGISTERING,
    NBIOT_STATUS_REJECT,
    NBIOT_STATUS_UNKNOWN,
    NBIOT_STATUS_ROAMING,
} nbiot_reg_status_t;

typedef struct {
    int rssi;
    int ber;
} nbiot_sig_qual_resp_t;

typedef struct {
    int n;
    int stat;
} nbiot_regs_stat_resp_t;

int u1_at0(void);
int u1_get_imei(nbiot_imei_t *pimei);
int u1_get_imsi(nbiot_imsi_t *pimsi);
int u1_get_iccid(nbiot_iccid_t *piccid);
int u1_start_nbiot(void);
int u1_stop_nbiot(void);
int u1_reset_nbiot(void);
int u1_set_signal_strength_ind(int status);
int u1_set_register_status_ind(int status);
int u1_check_register_status(nbiot_regs_stat_resp_t *respond);
int u1_check_signal_quality(nbiot_sig_qual_resp_t *respond);
int u1_check_simcard_is_insert(int *pstatus);
int u1_get_local_ip(char ip[16]);
int u1_get_cell_info(nbiot_cell_info_t *pcellinfo);
int u1_module_init(utask_t *task, u1_nbiot_param_t *param);

#ifdef __cplusplus
}
#endif

#endif
