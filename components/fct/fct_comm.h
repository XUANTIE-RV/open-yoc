/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#ifndef _FCT_COMM_H_

#include <stdio.h>

typedef enum {
    TEST_IDLE = 0,
    TEST_RUNNING,
    TEST_END,
    TEST_ERROR,
} fct_test_stat_t;

int fct_comm_init(int uart_id, uint32_t baud_rate);
int fct_comm_unreg_cmd(const char *name);
int fct_comm_reg_cmd(const char *name, const char *help, int cmd_id);

int fct_state();
char *fct_get_result(void);
int fct_run_test(int cmd_id, int isquery, int argc, char **argv);
#endif
