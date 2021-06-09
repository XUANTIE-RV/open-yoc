/*
 * Copyright (C) 2019-2030 Alibaba Group Holding Limited
 */

#include <aos/cli.h>

void board_cli_init()
{
    aos_cli_init();

    extern void cli_reg_cmd_ps(void);
    cli_reg_cmd_ps();

    extern void cli_reg_cmd_free(void);
    cli_reg_cmd_free();

    extern void cli_reg_cmd_kvtest(void);
    cli_reg_cmd_kvtest();

    extern void test_yunit_test_register_cmd(void);
    test_yunit_test_register_cmd();

}
