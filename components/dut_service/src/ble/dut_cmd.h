/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#ifndef __DUT_CMD_BLE_H_
#define __DUT_CMD_BLE_H_

#include "dut_service.h"

int dut_cmd_tx_single_tone(dut_cmd_type_e type, int argc, char *argv[]);

int dut_cmd_tx_mod_burst(dut_cmd_type_e type, int argc, char *argv[]);

int dut_cmd_rx_demod_burst(dut_cmd_type_e type, int argc, char *argv[]);

int dut_cmd_sleep(dut_cmd_type_e type, int argc, char *argv[]);

int dut_cmd_opt_mac(dut_cmd_type_e type, int argc, char *argv[]);

int dut_cmd_xtal_cap(dut_cmd_type_e type, int argc, char *argv[]);

int dut_cmd_transmit_stop(dut_cmd_type_e type, int argc, char *argv[]);

#endif
