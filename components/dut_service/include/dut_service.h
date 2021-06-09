/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#ifndef __DUT_SERVICE_H_
#define __DUT_SERVICE_H_

#ifndef CONFIG_DUT_UART_BUF_SIZE
#define CONFIG_DUT_UART_BUF_SIZE 128
#endif

#ifndef CONFIG_DUT_AT_CMD_MAX_NUM
#define CONFIG_DUT_AT_CMD_MAX_NUM 10
#endif

typedef enum {
    DUT_CMD_EXECUTE  = 0x1,
    DUT_CMD_GET      = 0x2,
    DUT_CMD_HELP     = 0x3,
} dut_cmd_type_e;

typedef struct _dut_service_cfg_t {
    int uart_idx;
    int uart_baud;
} dut_service_cfg_t;

typedef int (*dut_cmd_function_t)(dut_cmd_type_e type, int argc, char *argv[]);

typedef struct _dut_at_cmd_t {
    const char *name;
    dut_cmd_function_t function;
    const char *help;
} dut_at_cmd_t;

int dut_service_init(dut_service_cfg_t *config);

void dut_task_entry(void);

int dut_at_send(const char *command, ...);

int dut_service_cmds_reg(const dut_at_cmd_t commands[], int num_commands);

int dut_service_cmds_unreg(const dut_at_cmd_t commands[], int num_commands);

int dut_service_cmd_reg(const dut_at_cmd_t *info);

int dut_service_cmd_unreg(const dut_at_cmd_t *info);

//////////////
/* Default cmds for ble */
int dut_ble_default_cmds_reg(void);

#endif
