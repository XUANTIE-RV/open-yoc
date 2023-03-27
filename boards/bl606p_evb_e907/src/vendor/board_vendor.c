/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */
#include <board.h>

extern void cli_reg_cmd_board_ext(void);
extern void cli_reg_cmd_mfg(void);
extern void cli_reg_cmd_isp(void);
extern void cli_reg_cmd_codec(void);
extern void cli_reg_cmd_peripherals(void);
extern void cli_reg_cmd_bt_stack(void);

void board_vendor_init(void)
{
    cli_reg_cmd_board_ext();
    cli_reg_cmd_mfg();
    cli_reg_cmd_isp();
	cli_reg_cmd_codec();
#if defined(CONFIG_BOARD_BT) && CONFIG_BOARD_BT
    cli_reg_cmd_bt_stack();
#endif
    //cli_reg_cmd_peripherals();
}
