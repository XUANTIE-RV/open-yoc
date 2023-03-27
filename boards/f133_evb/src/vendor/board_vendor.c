/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

extern void cli_reg_cmd_board_specific(void);
extern void cli_reg_cmd_bt_stack(void);
extern void wifidrv_register_iwpriv_cmd(void);
void board_vendor_init(void)
{
#if defined(CONFIG_BOARD_BT) && CONFIG_BOARD_BT
    cli_reg_cmd_bt_stack();
#endif
#if defined(CONFIG_BOARD_WIFI) && CONFIG_BOARD_WIFI
    wifidrv_register_iwpriv_cmd();
#endif
    cli_reg_cmd_board_specific();
}
